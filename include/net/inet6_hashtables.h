/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the BSD Socket
 *		interface as the means of communication with the user level.
 *
 * Authors:	Lotsa people, from code originally in tcp
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _INET6_HASHTABLES_H
#define _INET6_HASHTABLES_H


#if IS_ENABLED(CONFIG_IPV6)
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/types.h>
#include <linux/jhash.h>

#include <net/inet_sock.h>

#include <net/ipv6.h>
#include <net/netns/hash.h>

struct inet_hashinfo;

<<<<<<< HEAD
static inline unsigned int inet6_ehashfn(struct net *net,
				const struct in6_addr *laddr, const u16 lport,
				const struct in6_addr *faddr, const __be16 fport)
{
	u32 ports = (((u32)lport) << 16) | (__force u32)fport;

	return jhash_3words((__force u32)laddr->s6_addr32[3],
			    ipv6_addr_jhash(faddr),
			    ports,
			    inet_ehash_secret + net_hash_mix(net));
}

static inline int inet6_sk_ehashfn(const struct sock *sk)
{
	const struct inet_sock *inet = inet_sk(sk);
	const struct ipv6_pinfo *np = inet6_sk(sk);
	const struct in6_addr *laddr = &np->rcv_saddr;
	const struct in6_addr *faddr = &np->daddr;
	const __u16 lport = inet->inet_num;
	const __be16 fport = inet->inet_dport;
	struct net *net = sock_net(sk);

	return inet6_ehashfn(net, laddr, lport, faddr, fport);
=======
static inline unsigned int __inet6_ehashfn(const u32 lhash,
				    const u16 lport,
				    const u32 fhash,
				    const __be16 fport,
				    const u32 initval)
{
	const u32 ports = (((u32)lport) << 16) | (__force u32)fport;
	return jhash_3words(lhash, fhash, ports, initval);
>>>>>>> android-4.9
}

/*
 * Sockets in TCP_CLOSE state are _always_ taken out of the hash, so
 * we need not check it for TCP lookups anymore, thanks Alexey. -DaveM
 *
 * The sockhash lock must be held as a reader here.
 */
struct sock *__inet6_lookup_established(struct net *net,
					struct inet_hashinfo *hashinfo,
					const struct in6_addr *saddr,
					const __be16 sport,
					const struct in6_addr *daddr,
					const u16 hnum, const int dif);

struct sock *inet6_lookup_listener(struct net *net,
				   struct inet_hashinfo *hashinfo,
				   struct sk_buff *skb, int doff,
				   const struct in6_addr *saddr,
				   const __be16 sport,
				   const struct in6_addr *daddr,
				   const unsigned short hnum, const int dif);

static inline struct sock *__inet6_lookup(struct net *net,
					  struct inet_hashinfo *hashinfo,
					  struct sk_buff *skb, int doff,
					  const struct in6_addr *saddr,
					  const __be16 sport,
					  const struct in6_addr *daddr,
					  const u16 hnum,
					  const int dif,
					  bool *refcounted)
{
	struct sock *sk = __inet6_lookup_established(net, hashinfo, saddr,
						sport, daddr, hnum, dif);
	*refcounted = true;
	if (sk)
		return sk;
	*refcounted = false;
	return inet6_lookup_listener(net, hashinfo, skb, doff, saddr, sport,
				     daddr, hnum, dif);
}

static inline struct sock *__inet6_lookup_skb(struct inet_hashinfo *hashinfo,
					      struct sk_buff *skb, int doff,
					      const __be16 sport,
					      const __be16 dport,
					      int iif,
					      bool *refcounted)
{
	struct sock *sk = skb_steal_sock(skb);

	*refcounted = true;
	if (sk)
		return sk;

	return __inet6_lookup(dev_net(skb_dst(skb)->dev), hashinfo, skb,
			      doff, &ipv6_hdr(skb)->saddr, sport,
			      &ipv6_hdr(skb)->daddr, ntohs(dport),
			      iif, refcounted);
}

struct sock *inet6_lookup(struct net *net, struct inet_hashinfo *hashinfo,
			  struct sk_buff *skb, int doff,
			  const struct in6_addr *saddr, const __be16 sport,
			  const struct in6_addr *daddr, const __be16 dport,
			  const int dif);

int inet6_hash(struct sock *sk);
#endif /* IS_ENABLED(CONFIG_IPV6) */

#define INET6_MATCH(__sk, __net, __saddr, __daddr, __ports, __dif)	\
	(((__sk)->sk_portpair == (__ports))			&&	\
	 ((__sk)->sk_family == AF_INET6)			&&	\
	 ipv6_addr_equal(&(__sk)->sk_v6_daddr, (__saddr))		&&	\
	 ipv6_addr_equal(&(__sk)->sk_v6_rcv_saddr, (__daddr))	&&	\
	 (!(__sk)->sk_bound_dev_if	||				\
	   ((__sk)->sk_bound_dev_if == (__dif))) 		&&	\
	 net_eq(sock_net(__sk), (__net)))

#endif /* _INET6_HASHTABLES_H */
