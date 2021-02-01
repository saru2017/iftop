/*
 * resolver.c:
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "ns_hash.h"
#include "iftop.h"

#include "threadprof.h"

#include "options.h"


#define RESOLVE_QUEUE_LENGTH 20

struct addr_storage {
    int af;                     /* AF_INET or AF_INET6 */
    int len;                    /* sizeof(struct in_addr or in6_addr) */
    union {
        struct in_addr  addr4;
        struct in6_addr addr6;
    } addr;
#define as_addr4 addr.addr4
#define as_addr6 addr.addr6
};

struct addr_storage resolve_queue[RESOLVE_QUEUE_LENGTH];

pthread_cond_t resolver_queue_cond;
pthread_mutex_t resolver_queue_mutex;

hash_type* ns_hash;

int head;
int tail;

extern options_t options;


/* 
 * We have a choice of resolver methods. Real computers have getnameinfo or
 * gethostbyaddr_r, which are reentrant and therefore thread safe. Other
 * machines don't, and so we can use non-reentrant gethostbyaddr and have only
 * one resolver thread.  Alternatively, we can use the MIT ares asynchronous
 * DNS library to do this.
 */


/**
 * Implementation using gethostbyname. Since this is nonreentrant, we have to
 * wrap it in a mutex, losing all benefit of multithreaded resolution.
 */
char *do_resolve(struct addr_storage *addr) {
    static pthread_mutex_t ghba_mtx = PTHREAD_MUTEX_INITIALIZER;
    char *s = NULL;
    struct hostent *he;
    pthread_mutex_lock(&ghba_mtx);
    he = gethostbyaddr((char*)&addr->addr, addr->len, addr->af);
    if (he)
        s = xstrdup(he->h_name);
    pthread_mutex_unlock(&ghba_mtx);
    return s;
}




void resolver_worker(void* ptr) {
/*    int thread_number = *(int*)ptr;*/
    pthread_mutex_lock(&resolver_queue_mutex);
    sethostent(1);
    while(1) {
        /* Wait until we are told that an address has been added to the 
         * queue. */
        pthread_cond_wait(&resolver_queue_cond, &resolver_queue_mutex);

        /* Keep resolving until the queue is empty */
        while(head != tail) {
            char * hostname;
            struct addr_storage addr = resolve_queue[tail];

            /* mutex always locked at this point */

            tail = (tail + 1) % RESOLVE_QUEUE_LENGTH;

            pthread_mutex_unlock(&resolver_queue_mutex);

            hostname = do_resolve(&addr);

            /*
             * Store the result in ns_hash
             */
            pthread_mutex_lock(&resolver_queue_mutex);

            if(hostname != NULL) {
                char* old;
		union {
		    char **ch_pp;
		    void **void_pp;
		} u_old = { &old };
                if(hash_find(ns_hash, &addr.as_addr6, u_old.void_pp) == HASH_STATUS_OK) {
                    hash_delete(ns_hash, &addr);
                    xfree(old);
                }
                hash_insert(ns_hash, &addr.as_addr6, (void*)hostname);
            }

        }
    }
}

void resolver_initialise() {
    int* n;
    int i;
    pthread_t thread;
    head = tail = 0;

    ns_hash = ns_hash_create();
    
    pthread_mutex_init(&resolver_queue_mutex, NULL);
    pthread_cond_init(&resolver_queue_cond, NULL);

    for(i = 0; i < 2; i++) {
        n = (int*)xmalloc(sizeof *n);
        *n = i;
        pthread_create(&thread, NULL, (void*)&resolver_worker, (void*)n);
    }

}

void resolve(int af, void* addr, char* result, int buflen) {
    char* hostname;
    union {
	char **ch_pp;
	void **void_pp;
    } u_hostname = { &hostname };
    int added = 0;
    struct addr_storage *raddr;

    if(options.dnsresolution == 1) {

        raddr = malloc(sizeof *raddr);
        memset(raddr, 0, sizeof *raddr);
        raddr->af = af;
        raddr->len = (af == AF_INET ? sizeof(struct in_addr)
                      : sizeof(struct in6_addr));
        memcpy(&raddr->addr, addr, raddr->len);

        pthread_mutex_lock(&resolver_queue_mutex);

        if(hash_find(ns_hash, &raddr->as_addr6, u_hostname.void_pp) == HASH_STATUS_OK) {
            /* Found => already resolved, or on the queue, no need to keep
	     * it around */
            free(raddr);
        }
        else {
            hostname = xmalloc(INET6_ADDRSTRLEN);
            inet_ntop(af, &raddr->addr, hostname, INET6_ADDRSTRLEN);

            hash_insert(ns_hash, &raddr->as_addr6, hostname);

            if(((head + 1) % RESOLVE_QUEUE_LENGTH) == tail) {
                /* queue full */
            }
            else if ((af == AF_INET6)
                     && (IN6_IS_ADDR_LINKLOCAL(&raddr->as_addr6)
                         || IN6_IS_ADDR_SITELOCAL(&raddr->as_addr6))) {
                /* Link-local and site-local stay numerical. */
            }
            else {
                resolve_queue[head] = *raddr;
                head = (head + 1) % RESOLVE_QUEUE_LENGTH;
                added = 1;
            }
        }
        pthread_mutex_unlock(&resolver_queue_mutex);

        if(added == 1) {
            pthread_cond_signal(&resolver_queue_cond);
        }

        if(result != NULL && buflen > 1) {
            strncpy(result, hostname, buflen - 1);
            result[buflen - 1] = '\0';
        }
    }
}
