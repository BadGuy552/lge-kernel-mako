/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (c) 2000-2001, 2011-2012 The Linux Foundation.  All rights reserved.
   Copyright (C) 2009-2010 Gustavo F. Padovan <gustavo@padovan.org>
   Copyright (C) 2010 Google Inc.

   Written 2000,2001 by Maxim Krasnyansky <maxk@qualcomm.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

/* Bluetooth L2CAP sockets. */

<<<<<<< HEAD
#include <linux/interrupt.h>
#include <linux/module.h>
=======
#include <linux/module.h>
#include <linux/export.h>
>>>>>>> android-4.9

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/l2cap.h>
<<<<<<< HEAD
#include <net/bluetooth/smp.h>
#include <net/bluetooth/amp.h>

/* ---- L2CAP timers ---- */
static void l2cap_sock_timeout(unsigned long arg)
{
	struct sock *sk = (struct sock *) arg;
	int reason;

	BT_DBG("sock %p state %d", sk, sk->sk_state);

	bh_lock_sock(sk);

	if (sock_owned_by_user(sk)) {
		/* sk is owned by user. Try again later */
		l2cap_sock_set_timer(sk, HZ / 5);
		bh_unlock_sock(sk);
		sock_put(sk);
		return;
	}

	if (sk->sk_state == BT_CONNECTED || sk->sk_state == BT_CONFIG)
		reason = ECONNREFUSED;
	else if (sk->sk_state == BT_CONNECT &&
				l2cap_pi(sk)->sec_level != BT_SECURITY_SDP)
		reason = ECONNREFUSED;
	else
		reason = ETIMEDOUT;

	__l2cap_sock_close(sk, reason);

	bh_unlock_sock(sk);

	l2cap_sock_kill(sk);
	sock_put(sk);
}

void l2cap_sock_set_timer(struct sock *sk, long timeout)
{
	BT_DBG("sk %p state %d timeout %ld", sk, sk->sk_state, timeout);
	sk_reset_timer(sk, &sk->sk_timer, jiffies + timeout);
}

void l2cap_sock_clear_timer(struct sock *sk)
{
	BT_DBG("sock %p state %d", sk, sk->sk_state);
	sk_stop_timer(sk, &sk->sk_timer);
}

int l2cap_sock_le_params_valid(struct bt_le_params *le_params)
{
	if (!le_params || le_params->latency > BT_LE_LATENCY_MAX ||
			le_params->scan_window > BT_LE_SCAN_WINDOW_MAX ||
			le_params->scan_interval < BT_LE_SCAN_INTERVAL_MIN ||
			le_params->scan_window > le_params->scan_interval ||
			le_params->interval_min < BT_LE_CONN_INTERVAL_MIN ||
			le_params->interval_max > BT_LE_CONN_INTERVAL_MAX ||
			le_params->interval_min > le_params->interval_max ||
			le_params->supervision_timeout < BT_LE_SUP_TO_MIN ||
			le_params->supervision_timeout > BT_LE_SUP_TO_MAX) {
		return 0;
	}

	return 1;
}

int l2cap_sock_le_conn_update_params_valid(struct bt_le_params *le_params)
{
	if (!le_params || le_params->latency > BT_LE_LATENCY_MAX ||
			le_params->interval_min < BT_LE_CONN_INTERVAL_MIN ||
			le_params->interval_max > BT_LE_CONN_INTERVAL_MAX ||
			le_params->interval_min > le_params->interval_max ||
			le_params->supervision_timeout < BT_LE_SUP_TO_MIN ||
			le_params->supervision_timeout > BT_LE_SUP_TO_MAX) {
		return 0;
	}

	return 1;
}

static struct sock *__l2cap_get_sock_by_addr(__le16 psm, bdaddr_t *src)
{
	struct sock *sk;
	struct hlist_node *node;
	sk_for_each(sk, node, &l2cap_sk_list.head)
		if (l2cap_pi(sk)->sport == psm && !bacmp(&bt_sk(sk)->src, src))
			goto found;
	sk = NULL;
found:
	return sk;
=======

#include "smp.h"

static struct bt_sock_list l2cap_sk_list = {
	.lock = __RW_LOCK_UNLOCKED(l2cap_sk_list.lock)
};

static const struct proto_ops l2cap_sock_ops;
static void l2cap_sock_init(struct sock *sk, struct sock *parent);
static struct sock *l2cap_sock_alloc(struct net *net, struct socket *sock,
				     int proto, gfp_t prio, int kern);

bool l2cap_is_socket(struct socket *sock)
{
	return sock && sock->ops == &l2cap_sock_ops;
}
EXPORT_SYMBOL(l2cap_is_socket);

static int l2cap_validate_bredr_psm(u16 psm)
{
	/* PSM must be odd and lsb of upper byte must be 0 */
	if ((psm & 0x0101) != 0x0001)
		return -EINVAL;

	/* Restrict usage of well-known PSMs */
	if (psm < L2CAP_PSM_DYN_START && !capable(CAP_NET_BIND_SERVICE))
		return -EACCES;

	return 0;
}

static int l2cap_validate_le_psm(u16 psm)
{
	/* Valid LE_PSM ranges are defined only until 0x00ff */
	if (psm > L2CAP_PSM_LE_DYN_END)
		return -EINVAL;

	/* Restrict fixed, SIG assigned PSM values to CAP_NET_BIND_SERVICE */
	if (psm < L2CAP_PSM_LE_DYN_START && !capable(CAP_NET_BIND_SERVICE))
		return -EACCES;

	return 0;
>>>>>>> android-4.9
}

static int l2cap_sock_bind(struct socket *sock, struct sockaddr *addr, int alen)
{
	struct sock *sk = sock->sk;
	struct sockaddr_l2 la;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (!addr || addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	memset(&la, 0, sizeof(la));
	len = min_t(unsigned int, sizeof(la), alen);
	memcpy(&la, addr, len);

	if (la.l2_cid && la.l2_psm)
		return -EINVAL;

	if (!bdaddr_type_is_valid(la.l2_bdaddr_type))
		return -EINVAL;

	if (bdaddr_type_is_le(la.l2_bdaddr_type)) {
		/* We only allow ATT user space socket */
		if (la.l2_cid &&
		    la.l2_cid != cpu_to_le16(L2CAP_CID_ATT))
			return -EINVAL;
	}

	lock_sock(sk);

	if (sk->sk_state != BT_OPEN) {
		err = -EBADFD;
		goto done;
	}

	if (la.l2_psm) {
		__u16 psm = __le16_to_cpu(la.l2_psm);

		if (la.l2_bdaddr_type == BDADDR_BREDR)
			err = l2cap_validate_bredr_psm(psm);
		else
			err = l2cap_validate_le_psm(psm);

		if (err)
			goto done;
	}

<<<<<<< HEAD
	write_lock_bh(&l2cap_sk_list.lock);

	if (la.l2_psm && __l2cap_get_sock_by_addr(la.l2_psm, &la.l2_bdaddr)) {
		err = -EADDRINUSE;
	} else {
		/* Save source address */
		bacpy(&bt_sk(sk)->src, &la.l2_bdaddr);
		l2cap_pi(sk)->psm   = la.l2_psm;
		l2cap_pi(sk)->sport = la.l2_psm;
		sk->sk_state = BT_BOUND;

		if (__le16_to_cpu(la.l2_psm) == 0x0001 ||
					__le16_to_cpu(la.l2_psm) == 0x0003)
			l2cap_pi(sk)->sec_level = BT_SECURITY_SDP;
	}

	if (la.l2_cid)
		l2cap_pi(sk)->scid = la.l2_cid;
=======
	bacpy(&chan->src, &la.l2_bdaddr);
	chan->src_type = la.l2_bdaddr_type;

	if (la.l2_cid)
		err = l2cap_add_scid(chan, __le16_to_cpu(la.l2_cid));
	else
		err = l2cap_add_psm(chan, &la.l2_bdaddr, la.l2_psm);

	if (err < 0)
		goto done;

	switch (chan->chan_type) {
	case L2CAP_CHAN_CONN_LESS:
		if (__le16_to_cpu(la.l2_psm) == L2CAP_PSM_3DSP)
			chan->sec_level = BT_SECURITY_SDP;
		break;
	case L2CAP_CHAN_CONN_ORIENTED:
		if (__le16_to_cpu(la.l2_psm) == L2CAP_PSM_SDP ||
		    __le16_to_cpu(la.l2_psm) == L2CAP_PSM_RFCOMM)
			chan->sec_level = BT_SECURITY_SDP;
		break;
	case L2CAP_CHAN_RAW:
		chan->sec_level = BT_SECURITY_SDP;
		break;
	case L2CAP_CHAN_FIXED:
		/* Fixed channels default to the L2CAP core not holding a
		 * hci_conn reference for them. For fixed channels mapping to
		 * L2CAP sockets we do want to hold a reference so set the
		 * appropriate flag to request it.
		 */
		set_bit(FLAG_HOLD_HCI_CONN, &chan->flags);
		break;
	}

	if (chan->psm && bdaddr_type_is_le(chan->src_type))
		chan->mode = L2CAP_MODE_LE_FLOWCTL;
>>>>>>> android-4.9

	write_unlock_bh(&l2cap_sk_list.lock);

done:
	release_sock(sk);
	return err;
}

static int l2cap_sock_connect(struct socket *sock, struct sockaddr *addr,
			      int alen, int flags)
{
	struct sock *sk = sock->sk;
	struct sockaddr_l2 la;
	int len, err = 0;

	BT_DBG("sk %p type %d mode %d state %d", sk, sk->sk_type,
		l2cap_pi(sk)->mode, sk->sk_state);

	if (!addr || alen < sizeof(addr->sa_family) ||
		addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	memset(&la, 0, sizeof(la));
	len = min_t(unsigned int, sizeof(la), alen);
	memcpy(&la, addr, len);

	if (la.l2_cid && la.l2_psm)
		return -EINVAL;

<<<<<<< HEAD
=======
	if (!bdaddr_type_is_valid(la.l2_bdaddr_type))
		return -EINVAL;

	/* Check that the socket wasn't bound to something that
	 * conflicts with the address given to connect(). If chan->src
	 * is BDADDR_ANY it means bind() was never used, in which case
	 * chan->src_type and la.l2_bdaddr_type do not need to match.
	 */
	if (chan->src_type == BDADDR_BREDR && bacmp(&chan->src, BDADDR_ANY) &&
	    bdaddr_type_is_le(la.l2_bdaddr_type)) {
		/* Old user space versions will try to incorrectly bind
		 * the ATT socket using BDADDR_BREDR. We need to accept
		 * this and fix up the source address type only when
		 * both the source CID and destination CID indicate
		 * ATT. Anything else is an invalid combination.
		 */
		if (chan->scid != L2CAP_CID_ATT ||
		    la.l2_cid != cpu_to_le16(L2CAP_CID_ATT))
			return -EINVAL;

		/* We don't have the hdev available here to make a
		 * better decision on random vs public, but since all
		 * user space versions that exhibit this issue anyway do
		 * not support random local addresses assuming public
		 * here is good enough.
		 */
		chan->src_type = BDADDR_LE_PUBLIC;
	}

	if (chan->src_type != BDADDR_BREDR && la.l2_bdaddr_type == BDADDR_BREDR)
		return -EINVAL;

	if (bdaddr_type_is_le(la.l2_bdaddr_type)) {
		/* We only allow ATT user space socket */
		if (la.l2_cid &&
		    la.l2_cid != cpu_to_le16(L2CAP_CID_ATT))
			return -EINVAL;
	}

	if (chan->psm && bdaddr_type_is_le(chan->src_type))
		chan->mode = L2CAP_MODE_LE_FLOWCTL;

	err = l2cap_chan_connect(chan, la.l2_psm, __le16_to_cpu(la.l2_cid),
				 &la.l2_bdaddr, la.l2_bdaddr_type);
	if (err)
		return err;

>>>>>>> android-4.9
	lock_sock(sk);

	if ((sk->sk_type == SOCK_SEQPACKET || sk->sk_type == SOCK_STREAM)
		&& !(la.l2_psm || la.l2_cid || l2cap_pi(sk)->fixed_channel)) {
		err = -EINVAL;
		goto done;
	}

	switch (l2cap_pi(sk)->mode) {
	case L2CAP_MODE_BASIC:
		break;
	case L2CAP_MODE_ERTM:
	case L2CAP_MODE_STREAMING:
		if (!disable_ertm)
			break;
		/* fall through */
	default:
		err = -ENOTSUPP;
		goto done;
	}

	switch (sk->sk_state) {
	case BT_CONNECT:
	case BT_CONNECT2:
	case BT_CONFIG:
		/* Already connecting */
		goto wait;

	case BT_CONNECTED:
		/* Already connected */
		err = -EISCONN;
		goto done;

	case BT_OPEN:
	case BT_BOUND:
		/* Can connect */
		break;

	default:
		err = -EBADFD;
		goto done;
	}

	/* PSM must be odd and lsb of upper byte must be 0 */
	if ((__le16_to_cpu(la.l2_psm) & 0x0101) != 0x0001 &&
		!l2cap_pi(sk)->fixed_channel &&
				sk->sk_type != SOCK_RAW && !la.l2_cid) {
		BT_DBG("Bad PSM 0x%x", (int)__le16_to_cpu(la.l2_psm));
		err = -EINVAL;
		goto done;
	}

	/* Set destination address and psm */
	bacpy(&bt_sk(sk)->dst, &la.l2_bdaddr);
	l2cap_pi(sk)->psm = la.l2_psm;
	l2cap_pi(sk)->dcid = la.l2_cid;

	err = l2cap_do_connect(sk);
	if (err)
		goto done;

wait:
	err = bt_sock_wait_state(sk, BT_CONNECTED,
<<<<<<< HEAD
			sock_sndtimeo(sk, flags & O_NONBLOCK));
done:
	if (err)
		BT_ERR("failed %d", err);
=======
				 sock_sndtimeo(sk, flags & O_NONBLOCK));

>>>>>>> android-4.9
	release_sock(sk);
	return err;
}

static int l2cap_sock_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sk %p backlog %d", sk, backlog);

	lock_sock(sk);

	if (sk->sk_state != BT_BOUND) {
		err = -EBADFD;
		goto done;
	}

<<<<<<< HEAD
	switch (l2cap_pi(sk)->mode) {
=======
	if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM) {
		err = -EINVAL;
		goto done;
	}

	switch (chan->mode) {
>>>>>>> android-4.9
	case L2CAP_MODE_BASIC:
	case L2CAP_MODE_LE_FLOWCTL:
		break;
	case L2CAP_MODE_ERTM:
	case L2CAP_MODE_STREAMING:
		if (!disable_ertm)
			break;
		/* fall through */
	default:
		err = -EOPNOTSUPP;
		goto done;
	}

	if (!l2cap_pi(sk)->psm && !l2cap_pi(sk)->scid) {
		bdaddr_t *src = &bt_sk(sk)->src;
		u16 psm;

		err = -EINVAL;

		write_lock_bh(&l2cap_sk_list.lock);

		for (psm = 0x1001; psm < 0x1100; psm += 2)
			if (!__l2cap_get_sock_by_addr(cpu_to_le16(psm), src)) {
				l2cap_pi(sk)->psm   = cpu_to_le16(psm);
				l2cap_pi(sk)->sport = cpu_to_le16(psm);
				err = 0;
				break;
			}

		write_unlock_bh(&l2cap_sk_list.lock);

		if (err < 0)
			goto done;
	}

	sk->sk_max_ack_backlog = backlog;
	sk->sk_ack_backlog = 0;
<<<<<<< HEAD
=======

	/* Listening channels need to use nested locking in order not to
	 * cause lockdep warnings when the created child channels end up
	 * being locked in the same thread as the parent channel.
	 */
	atomic_set(&chan->nesting, L2CAP_NESTING_PARENT);

	chan->state = BT_LISTEN;
>>>>>>> android-4.9
	sk->sk_state = BT_LISTEN;

done:
	release_sock(sk);
	return err;
}

static int l2cap_sock_accept(struct socket *sock, struct socket *newsock,
			     int flags)
{
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	struct sock *sk = sock->sk, *nsk;
	long timeo;
	int err = 0;

	lock_sock_nested(sk, L2CAP_NESTING_PARENT);

	if (sk->sk_state != BT_LISTEN) {
		err = -EBADFD;
		goto done;
	}

	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);

	BT_DBG("sk %p timeo %ld", sk, timeo);

	/* Wait for an incoming connection. (wake-one). */
	add_wait_queue_exclusive(sk_sleep(sk), &wait);
<<<<<<< HEAD
	while (!(nsk = bt_accept_dequeue(sk, newsock))) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (!timeo) {
			err = -EAGAIN;
=======
	while (1) {
		if (sk->sk_state != BT_LISTEN) {
			err = -EBADFD;
>>>>>>> android-4.9
			break;
		}

		release_sock(sk);
		timeo = schedule_timeout(timeo);
		lock_sock_nested(sk, SINGLE_DEPTH_NESTING);

		if (sk->sk_state != BT_LISTEN) {
			err = -EBADFD;
			break;
		}

		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			break;
		}
<<<<<<< HEAD
	}
	set_current_state(TASK_RUNNING);
=======

		release_sock(sk);

		timeo = wait_woken(&wait, TASK_INTERRUPTIBLE, timeo);

		lock_sock_nested(sk, L2CAP_NESTING_PARENT);
	}
>>>>>>> android-4.9
	remove_wait_queue(sk_sleep(sk), &wait);

	if (err)
		goto done;

	newsock->state = SS_CONNECTED;

	BT_DBG("new socket %p", nsk);

done:
	release_sock(sk);
	return err;
}

static int l2cap_sock_getname(struct socket *sock, struct sockaddr *addr,
			      int *len, int peer)
{
	struct sockaddr_l2 *la = (struct sockaddr_l2 *) addr;
	struct sock *sk = sock->sk;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (peer && sk->sk_state != BT_CONNECTED &&
	    sk->sk_state != BT_CONNECT && sk->sk_state != BT_CONNECT2 &&
	    sk->sk_state != BT_CONFIG)
		return -ENOTCONN;

	memset(la, 0, sizeof(struct sockaddr_l2));
	addr->sa_family = AF_BLUETOOTH;
	*len = sizeof(struct sockaddr_l2);

	la->l2_psm = chan->psm;

	if (peer) {
<<<<<<< HEAD
		la->l2_psm = l2cap_pi(sk)->psm;
		bacpy(&la->l2_bdaddr, &bt_sk(sk)->dst);
		la->l2_cid = cpu_to_le16(l2cap_pi(sk)->dcid);
	} else {
		la->l2_psm = l2cap_pi(sk)->sport;
		bacpy(&la->l2_bdaddr, &bt_sk(sk)->src);
		la->l2_cid = cpu_to_le16(l2cap_pi(sk)->scid);
=======
		bacpy(&la->l2_bdaddr, &chan->dst);
		la->l2_cid = cpu_to_le16(chan->dcid);
		la->l2_bdaddr_type = chan->dst_type;
	} else {
		bacpy(&la->l2_bdaddr, &chan->src);
		la->l2_cid = cpu_to_le16(chan->scid);
		la->l2_bdaddr_type = chan->src_type;
>>>>>>> android-4.9
	}

	return 0;
}

static int l2cap_sock_getsockopt_old(struct socket *sock, int optname,
				     char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct l2cap_options opts;
	struct l2cap_conninfo cinfo;
	int len, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case L2CAP_OPTIONS:
		/* LE sockets should use BT_SNDMTU/BT_RCVMTU, but since
		 * legacy ATT code depends on getsockopt for
		 * L2CAP_OPTIONS we need to let this pass.
		 */
		if (bdaddr_type_is_le(chan->src_type) &&
		    chan->scid != L2CAP_CID_ATT) {
			err = -EINVAL;
			break;
		}

		memset(&opts, 0, sizeof(opts));
		opts.imtu     = l2cap_pi(sk)->imtu;
		opts.omtu     = l2cap_pi(sk)->omtu;
		opts.flush_to = l2cap_pi(sk)->flush_to;
		opts.mode     = l2cap_pi(sk)->mode;
		opts.fcs      = l2cap_pi(sk)->fcs;
		opts.max_tx   = l2cap_pi(sk)->max_tx;
		opts.txwin_size = l2cap_pi(sk)->tx_win;

		len = min_t(unsigned int, len, sizeof(opts));
		if (copy_to_user(optval, (char *) &opts, len))
			err = -EFAULT;

		break;

	case L2CAP_LM:
		switch (l2cap_pi(sk)->sec_level) {
		case BT_SECURITY_LOW:
			opt = L2CAP_LM_AUTH;
			break;
		case BT_SECURITY_MEDIUM:
			opt = L2CAP_LM_AUTH | L2CAP_LM_ENCRYPT;
			break;
		case BT_SECURITY_HIGH:
			opt = L2CAP_LM_AUTH | L2CAP_LM_ENCRYPT |
			      L2CAP_LM_SECURE;
			break;
		case BT_SECURITY_FIPS:
			opt = L2CAP_LM_AUTH | L2CAP_LM_ENCRYPT |
			      L2CAP_LM_SECURE | L2CAP_LM_FIPS;
			break;
		default:
			opt = 0;
			break;
		}

		if (l2cap_pi(sk)->role_switch)
			opt |= L2CAP_LM_MASTER;

		if (l2cap_pi(sk)->force_reliable)
			opt |= L2CAP_LM_RELIABLE;

		if (l2cap_pi(sk)->flushable)
			opt |= L2CAP_LM_FLUSHABLE;

		if (put_user(opt, (u32 __user *) optval))
			err = -EFAULT;

		break;

	case L2CAP_CONNINFO:
		if (sk->sk_state != BT_CONNECTED &&
		    !(sk->sk_state == BT_CONNECT2 &&
		      test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags))) {
			err = -ENOTCONN;
			break;
		}

		cinfo.hci_handle = l2cap_pi(sk)->conn->hcon->handle;
		memcpy(cinfo.dev_class, l2cap_pi(sk)->conn->hcon->dev_class, 3);

		len = min_t(unsigned int, len, sizeof(cinfo));
		if (copy_to_user(optval, (char *) &cinfo, len))
			err = -EFAULT;

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int l2cap_sock_getsockopt(struct socket *sock, int level, int optname,
				 char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct bt_security sec;
	struct bt_power pwr;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (level == SOL_L2CAP)
		return l2cap_sock_getsockopt_old(sock, optname, optval, optlen);

	if (level != SOL_BLUETOOTH)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case BT_SECURITY:
<<<<<<< HEAD
		if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM
				&& sk->sk_type != SOCK_RAW) {
=======
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
		    chan->chan_type != L2CAP_CHAN_FIXED &&
		    chan->chan_type != L2CAP_CHAN_RAW) {
>>>>>>> android-4.9
			err = -EINVAL;
			break;
		}

		memset(&sec, 0, sizeof(sec));
<<<<<<< HEAD
		sec.level = l2cap_pi(sk)->sec_level;

		if (sk->sk_state == BT_CONNECTED) {
			sec.key_size = l2cap_pi(sk)->conn->hcon->enc_key_size;
			sec.level = l2cap_pi(sk)->conn->hcon->sec_level;
=======
		if (chan->conn) {
			sec.level = chan->conn->hcon->sec_level;

			if (sk->sk_state == BT_CONNECTED)
				sec.key_size = chan->conn->hcon->enc_key_size;
		} else {
			sec.level = chan->sec_level;
>>>>>>> android-4.9
		}

		len = min_t(unsigned int, len, sizeof(sec));
		if (copy_to_user(optval, (char *) &sec, len))
			err = -EFAULT;

		break;

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (put_user(test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags),
			     (u32 __user *) optval))
			err = -EFAULT;

		break;

<<<<<<< HEAD
=======
	case BT_FLUSHABLE:
		if (put_user(test_bit(FLAG_FLUSHABLE, &chan->flags),
			     (u32 __user *) optval))
			err = -EFAULT;

		break;

>>>>>>> android-4.9
	case BT_POWER:
		if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM
		    && sk->sk_type != SOCK_RAW) {
			err = -EINVAL;
			break;
		}

		pwr.force_active = l2cap_pi(sk)->force_active;

		len = min_t(unsigned int, len, sizeof(pwr));
		if (copy_to_user(optval, (char *) &pwr, len))
			err = -EFAULT;

		break;

<<<<<<< HEAD
	case BT_AMP_POLICY:
		if (put_user(l2cap_pi(sk)->amp_pref, (u32 __user *) optval))
			err = -EFAULT;
		break;

	case BT_LE_PARAMS:
		if (l2cap_pi(sk)->scid != L2CAP_CID_LE_DATA) {
=======
	case BT_CHANNEL_POLICY:
		if (put_user(chan->chan_policy, (u32 __user *) optval))
			err = -EFAULT;
		break;

	case BT_SNDMTU:
		if (!bdaddr_type_is_le(chan->src_type)) {
>>>>>>> android-4.9
			err = -EINVAL;
			break;
		}

<<<<<<< HEAD
		if (copy_to_user(optval, (char *) &bt_sk(sk)->le_params,
						sizeof(bt_sk(sk)->le_params)))
=======
		if (sk->sk_state != BT_CONNECTED) {
			err = -ENOTCONN;
			break;
		}

		if (put_user(chan->omtu, (u16 __user *) optval))
			err = -EFAULT;
		break;

	case BT_RCVMTU:
		if (!bdaddr_type_is_le(chan->src_type)) {
			err = -EINVAL;
			break;
		}

		if (put_user(chan->imtu, (u16 __user *) optval))
>>>>>>> android-4.9
			err = -EFAULT;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static bool l2cap_valid_mtu(struct l2cap_chan *chan, u16 mtu)
{
	switch (chan->scid) {
	case L2CAP_CID_ATT:
		if (mtu < L2CAP_LE_MIN_MTU)
			return false;
		break;

	default:
		if (mtu < L2CAP_DEFAULT_MIN_MTU)
			return false;
	}

	return true;
}

static int l2cap_sock_setsockopt_old(struct socket *sock, int optname,
				     char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct l2cap_options opts;
	int len, le_sock, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	lock_sock(sk);

	le_sock = l2cap_pi(sk)->scid == L2CAP_CID_LE_DATA;

	switch (optname) {
	case L2CAP_OPTIONS:
<<<<<<< HEAD
		if (sk->sk_state == BT_CONNECTED && !le_sock) {
=======
		if (bdaddr_type_is_le(chan->src_type)) {
			err = -EINVAL;
			break;
		}

		if (sk->sk_state == BT_CONNECTED) {
>>>>>>> android-4.9
			err = -EINVAL;
			break;
		}

		opts.imtu     = l2cap_pi(sk)->imtu;
		opts.omtu     = l2cap_pi(sk)->omtu;
		opts.flush_to = l2cap_pi(sk)->flush_to;
		opts.mode     = l2cap_pi(sk)->mode;
		opts.fcs      = l2cap_pi(sk)->fcs;
		opts.max_tx   = l2cap_pi(sk)->max_tx;
		opts.txwin_size = l2cap_pi(sk)->tx_win;

		len = min_t(unsigned int, sizeof(opts), optlen);
		if (copy_from_user((char *) &opts, optval, len)) {
			err = -EFAULT;
			break;
		}

		if ((opts.imtu || opts.omtu) && le_sock &&
				(sk->sk_state == BT_CONNECTED)) {
			if (opts.imtu >= L2CAP_LE_DEFAULT_MTU)
				l2cap_pi(sk)->imtu = opts.imtu;
			if (opts.omtu >= L2CAP_LE_DEFAULT_MTU)
				l2cap_pi(sk)->omtu = opts.omtu;
			if (opts.imtu < L2CAP_LE_DEFAULT_MTU ||
					opts.omtu < L2CAP_LE_DEFAULT_MTU)
				err = -EINVAL;
			break;
		}

		if (opts.txwin_size < 1 ||
			opts.txwin_size > L2CAP_TX_WIN_MAX_EXTENDED) {
			err = -EINVAL;
			break;
		}

<<<<<<< HEAD
		l2cap_pi(sk)->mode = opts.mode;
		switch (l2cap_pi(sk)->mode) {
=======
		if (!l2cap_valid_mtu(chan, opts.imtu)) {
			err = -EINVAL;
			break;
		}

		chan->mode = opts.mode;
		switch (chan->mode) {
		case L2CAP_MODE_LE_FLOWCTL:
			break;
>>>>>>> android-4.9
		case L2CAP_MODE_BASIC:
			l2cap_pi(sk)->conf_state &= ~L2CAP_CONF_STATE2_DEVICE;
			break;
		case L2CAP_MODE_STREAMING:
			if (!disable_ertm) {
				/* No fallback to ERTM or Basic mode */
				l2cap_pi(sk)->conf_state |=
						L2CAP_CONF_STATE2_DEVICE;
				break;
			}
			err = -EINVAL;
			break;
		case L2CAP_MODE_ERTM:
			if (!disable_ertm)
				break;
			/* fall through */
		default:
			err = -EINVAL;
			break;
		}

<<<<<<< HEAD
		l2cap_pi(sk)->imtu = opts.imtu;
		l2cap_pi(sk)->omtu = opts.omtu;
		l2cap_pi(sk)->fcs  = opts.fcs;
		l2cap_pi(sk)->max_tx = opts.max_tx;
		l2cap_pi(sk)->tx_win = opts.txwin_size;
		l2cap_pi(sk)->flush_to = opts.flush_to;
=======
		chan->imtu = opts.imtu;
		chan->omtu = opts.omtu;
		chan->fcs  = opts.fcs;
		chan->max_tx = opts.max_tx;
		chan->tx_win = opts.txwin_size;
		chan->flush_to = opts.flush_to;
>>>>>>> android-4.9
		break;

	case L2CAP_LM:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt & L2CAP_LM_FIPS) {
			err = -EINVAL;
			break;
		}

		if (opt & L2CAP_LM_AUTH)
			l2cap_pi(sk)->sec_level = BT_SECURITY_LOW;
		if (opt & L2CAP_LM_ENCRYPT)
			l2cap_pi(sk)->sec_level = BT_SECURITY_MEDIUM;
		if (opt & L2CAP_LM_SECURE)
			l2cap_pi(sk)->sec_level = BT_SECURITY_HIGH;

		l2cap_pi(sk)->role_switch    = (opt & L2CAP_LM_MASTER);
		l2cap_pi(sk)->force_reliable = (opt & L2CAP_LM_RELIABLE);
		l2cap_pi(sk)->flushable = (opt & L2CAP_LM_FLUSHABLE);
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int l2cap_sock_setsockopt(struct socket *sock, int level, int optname,
				 char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct bt_security sec;
	struct bt_power pwr;
	struct bt_le_params le_params;
	struct l2cap_conn *conn;
	int len, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	if (level == SOL_L2CAP)
		return l2cap_sock_setsockopt_old(sock, optname, optval, optlen);

	if (level != SOL_BLUETOOTH)
		return -ENOPROTOOPT;

	lock_sock(sk);

	switch (optname) {
	case BT_SECURITY:
<<<<<<< HEAD
		if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM
				&& sk->sk_type != SOCK_RAW) {
=======
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
		    chan->chan_type != L2CAP_CHAN_FIXED &&
		    chan->chan_type != L2CAP_CHAN_RAW) {
>>>>>>> android-4.9
			err = -EINVAL;
			break;
		}

		sec.level = BT_SECURITY_LOW;

		len = min_t(unsigned int, sizeof(sec), optlen);
		if (copy_from_user((char *) &sec, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (sec.level < BT_SECURITY_LOW ||
		    sec.level > BT_SECURITY_FIPS) {
			err = -EINVAL;
			break;
		}

		l2cap_pi(sk)->sec_level = sec.level;

<<<<<<< HEAD
		conn = l2cap_pi(sk)->conn;
		if (conn && l2cap_pi(sk)->scid == L2CAP_CID_LE_DATA) {
			if (!conn->hcon->out) {
				err = -EINVAL;
				break;
			}

			if (smp_conn_security(conn, sec.level))
				break;

			err = 0;
			sk->sk_state = BT_CONFIG;
=======
		/*change security for LE channels */
		if (chan->scid == L2CAP_CID_ATT) {
			if (smp_conn_security(conn->hcon, sec.level))
				break;
			set_bit(FLAG_PENDING_SECURITY, &chan->flags);
			sk->sk_state = BT_CONFIG;
			chan->state = BT_CONFIG;

		/* or for ACL link */
		} else if ((sk->sk_state == BT_CONNECT2 &&
			    test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags)) ||
			   sk->sk_state == BT_CONNECTED) {
			if (!l2cap_chan_check_security(chan, true))
				set_bit(BT_SK_SUSPEND, &bt_sk(sk)->flags);
			else
				sk->sk_state_change(sk);
		} else {
			err = -EINVAL;
>>>>>>> android-4.9
		}
		break;

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt) {
			set_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
			set_bit(FLAG_DEFER_SETUP, &chan->flags);
		} else {
			clear_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
			clear_bit(FLAG_DEFER_SETUP, &chan->flags);
		}
		break;

	case BT_POWER:
		if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM
				&& sk->sk_type != SOCK_RAW) {
			err = -EINVAL;
			break;
		}

		pwr.force_active = 1;

		len = min_t(unsigned int, sizeof(pwr), optlen);
		if (copy_from_user((char *) &pwr, optval, len)) {
			err = -EFAULT;
			break;
		}
		l2cap_pi(sk)->force_active = pwr.force_active;
		break;

<<<<<<< HEAD
	case BT_AMP_POLICY:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if ((opt > BT_AMP_POLICY_PREFER_AMP) ||
			((l2cap_pi(sk)->mode != L2CAP_MODE_ERTM) &&
			 (l2cap_pi(sk)->mode != L2CAP_MODE_STREAMING))) {
=======
		if (opt == BT_FLUSHABLE_OFF) {
			conn = chan->conn;
			/* proceed further only when we have l2cap_conn and
			   No Flush support in the LM */
			if (!conn || !lmp_no_flush_capable(conn->hcon->hdev)) {
				err = -EINVAL;
				break;
			}
		}

		if (opt)
			set_bit(FLAG_FLUSHABLE, &chan->flags);
		else
			clear_bit(FLAG_FLUSHABLE, &chan->flags);
		break;

	case BT_POWER:
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
		    chan->chan_type != L2CAP_CHAN_RAW) {
>>>>>>> android-4.9
			err = -EINVAL;
			break;
		}

		l2cap_pi(sk)->amp_pref = (u8) opt;
		BT_DBG("BT_AMP_POLICY now %d", opt);

		if ((sk->sk_state == BT_CONNECTED) &&
			(l2cap_pi(sk)->amp_move_role == L2CAP_AMP_MOVE_NONE))
			l2cap_amp_move_init(sk);

		break;

	case BT_FLUSHABLE:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}
		l2cap_pi(sk)->flushable = opt;

		break;

<<<<<<< HEAD
	case BT_LE_PARAMS:
		if (l2cap_pi(sk)->scid != L2CAP_CID_LE_DATA) {
			err = -EINVAL;
			break;
		}

		if (copy_from_user((char *) &le_params, optval,
					sizeof(struct bt_le_params))) {
=======
	case BT_CHANNEL_POLICY:
		if (get_user(opt, (u32 __user *) optval)) {
>>>>>>> android-4.9
			err = -EFAULT;
			break;
		}

		conn = l2cap_pi(sk)->conn;
		if (!conn || !conn->hcon ||
				l2cap_pi(sk)->scid != L2CAP_CID_LE_DATA) {
			memcpy(&bt_sk(sk)->le_params, &le_params,
							sizeof(le_params));
			break;
		}

<<<<<<< HEAD
		if (!conn->hcon->out ||
				!l2cap_sock_le_conn_update_params_valid(
					&le_params)) {
			err = -EINVAL;
			break;
		}

		memcpy(&bt_sk(sk)->le_params, &le_params, sizeof(le_params));

		hci_le_conn_update(conn->hcon,
				le_params.interval_min,
				le_params.interval_max,
				le_params.latency,
				le_params.supervision_timeout);
=======
		if (chan->mode != L2CAP_MODE_ERTM &&
		    chan->mode != L2CAP_MODE_STREAMING) {
			err = -EOPNOTSUPP;
			break;
		}

		chan->chan_policy = (u8) opt;

		if (sk->sk_state == BT_CONNECTED &&
		    chan->move_role == L2CAP_MOVE_ROLE_NONE)
			l2cap_move_start(chan);

		break;

	case BT_SNDMTU:
		if (!bdaddr_type_is_le(chan->src_type)) {
			err = -EINVAL;
			break;
		}

		/* Setting is not supported as it's the remote side that
		 * decides this.
		 */
		err = -EPERM;
		break;

	case BT_RCVMTU:
		if (!bdaddr_type_is_le(chan->src_type)) {
			err = -EINVAL;
			break;
		}

		if (sk->sk_state == BT_CONNECTED) {
			err = -EISCONN;
			break;
		}

		if (get_user(opt, (u16 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		chan->imtu = opt;
>>>>>>> android-4.9
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int l2cap_sock_sendmsg(struct socket *sock, struct msghdr *msg,
			      size_t len)
{
	struct sock *sk = sock->sk;
	struct l2cap_pinfo *pi = l2cap_pi(sk);
	struct sk_buff *skb;
	struct sk_buff_head seg_queue;
	int err;
	u8 amp_id;

	BT_DBG("sock %p, sk %p", sock, sk);

	err = sock_error(sk);
	if (err)
		return err;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

<<<<<<< HEAD
	lock_sock(sk);

	if (sk->sk_state != BT_CONNECTED) {
		err = -ENOTCONN;
		goto done;
	}

	/* Connectionless channel */
	if (sk->sk_type == SOCK_DGRAM) {
		skb = l2cap_create_connless_pdu(sk, msg, len);
		if (IS_ERR(skb)) {
			err = PTR_ERR(skb);
		} else {
			l2cap_do_send(sk, skb);
			err = len;
		}
		goto done;
	}

	switch (pi->mode) {
	case L2CAP_MODE_BASIC:
		/* Check outgoing MTU */
		if (len > pi->omtu) {
			err = -EMSGSIZE;
			goto done;
		}

		/* Create a basic PDU */
		skb = l2cap_create_basic_pdu(sk, msg, len);
		if (IS_ERR(skb)) {
			err = PTR_ERR(skb);
			goto done;
		}

		l2cap_do_send(sk, skb);
		err = len;
		break;

	case L2CAP_MODE_ERTM:
	case L2CAP_MODE_STREAMING:

		/* Check outgoing MTU */
		if (len > pi->omtu) {
			err = -EMSGSIZE;
			goto done;
		}

		__skb_queue_head_init(&seg_queue);

		/* Do segmentation before calling in to the state machine,
		 * since it's possible to block while waiting for memory
		 * allocation.
		 */
		amp_id = pi->amp_id;
		err = l2cap_segment_sdu(sk, &seg_queue, msg, len, 0);

		/* The socket lock is released while segmenting, so check
		 * that the socket is still connected
		 */
		if (sk->sk_state != BT_CONNECTED) {
			__skb_queue_purge(&seg_queue);
			err = -ENOTCONN;
		}

		if (err) {
			BT_DBG("Error %d, sk_sndbuf %d, sk_wmem_alloc %d",
				err, sk->sk_sndbuf,
				atomic_read(&sk->sk_wmem_alloc));
			break;
		}

		if (pi->amp_id != amp_id) {
			/* Channel moved while unlocked. Resegment. */
			err = l2cap_resegment_queue(sk, &seg_queue);

			if (err)
				break;
		}

		if (pi->mode != L2CAP_MODE_STREAMING)
			err = l2cap_ertm_tx(sk, 0, &seg_queue,
				L2CAP_ERTM_EVENT_DATA_REQUEST);
		else
			err = l2cap_strm_tx(sk, &seg_queue);
		if (!err)
			err = len;

		/* If the skbs were not queued for sending, they'll still be in
		 * seg_queue and need to be purged.
		 */
		__skb_queue_purge(&seg_queue);
		break;

	default:
		BT_DBG("bad state %1.1x", pi->mode);
		err = -EBADFD;
	}

done:
=======
	if (sk->sk_state != BT_CONNECTED)
		return -ENOTCONN;

	lock_sock(sk);
	err = bt_sock_wait_ready(sk, msg->msg_flags);
>>>>>>> android-4.9
	release_sock(sk);
	if (err)
		return err;

	l2cap_chan_lock(chan);
	err = l2cap_chan_send(chan, msg, len);
	l2cap_chan_unlock(chan);

	return err;
}

static int l2cap_sock_recvmsg(struct socket *sock, struct msghdr *msg,
			      size_t len, int flags)
{
	struct sock *sk = sock->sk;
	int err;

	lock_sock(sk);

<<<<<<< HEAD
	if (sk->sk_state == BT_CONNECT2 && bt_sk(sk)->defer_setup) {
		struct l2cap_conn_rsp rsp;
		struct l2cap_conn *conn = l2cap_pi(sk)->conn;
		u8 buf[128];

		if (l2cap_pi(sk)->amp_id) {
			/* Physical link must be brought up before connection
			 * completes.
			 */
			amp_accept_physical(conn, l2cap_pi(sk)->amp_id, sk);
			release_sock(sk);
			return 0;
		}

		sk->sk_state = BT_CONFIG;

		rsp.scid   = cpu_to_le16(l2cap_pi(sk)->dcid);
		rsp.dcid   = cpu_to_le16(l2cap_pi(sk)->scid);
		rsp.result = cpu_to_le16(L2CAP_CR_SUCCESS);
		rsp.status = cpu_to_le16(L2CAP_CS_NO_INFO);
		l2cap_send_cmd(l2cap_pi(sk)->conn, l2cap_pi(sk)->ident,
					L2CAP_CONN_RSP, sizeof(rsp), &rsp);

		if (l2cap_pi(sk)->conf_state & L2CAP_CONF_REQ_SENT) {
			release_sock(sk);
			return 0;
		}

		l2cap_pi(sk)->conf_state |= L2CAP_CONF_REQ_SENT;
		l2cap_send_cmd(conn, l2cap_get_ident(conn), L2CAP_CONF_REQ,
				l2cap_build_conf_req(sk, buf), buf);
		l2cap_pi(sk)->num_conf_req++;

		release_sock(sk);
		return 0;
=======
	if (sk->sk_state == BT_CONNECT2 && test_bit(BT_SK_DEFER_SETUP,
						    &bt_sk(sk)->flags)) {
		if (bdaddr_type_is_le(pi->chan->src_type)) {
			sk->sk_state = BT_CONNECTED;
			pi->chan->state = BT_CONNECTED;
			__l2cap_le_connect_rsp_defer(pi->chan);
		} else {
			sk->sk_state = BT_CONFIG;
			pi->chan->state = BT_CONFIG;
			__l2cap_connect_rsp_defer(pi->chan);
		}

		err = 0;
		goto done;
>>>>>>> android-4.9
	}

	release_sock(sk);

	if (sock->type == SOCK_STREAM)
		err = bt_sock_stream_recvmsg(sock, msg, len, flags);
	else
		err = bt_sock_recvmsg(sock, msg, len, flags);

	if (err >= 0)
		l2cap_ertm_recv_done(sk);

<<<<<<< HEAD
=======
	lock_sock(sk);

	if (!test_bit(CONN_LOCAL_BUSY, &pi->chan->conn_state))
		goto done;

	if (pi->rx_busy_skb) {
		if (!__sock_queue_rcv_skb(sk, pi->rx_busy_skb))
			pi->rx_busy_skb = NULL;
		else
			goto done;
	}

	/* Restore data flow when half of the receive buffer is
	 * available.  This avoids resending large numbers of
	 * frames.
	 */
	if (atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf >> 1)
		l2cap_chan_busy(pi->chan, 0);

done:
	release_sock(sk);
>>>>>>> android-4.9
	return err;
}

/* Kill socket (only if zapped and orphan)
 * Must be called on unlocked socket.
 */
void l2cap_sock_kill(struct sock *sk)
{
	if (!sock_flag(sk, SOCK_ZAPPED) || sk->sk_socket)
		return;

	BT_DBG("sk %p state %d", sk, sk->sk_state);

	/* Kill poor orphan */
<<<<<<< HEAD
	bt_sock_unlink(&l2cap_sk_list, sk);
=======

	l2cap_chan_put(l2cap_pi(sk)->chan);
>>>>>>> android-4.9
	sock_set_flag(sk, SOCK_DEAD);
	sock_put(sk);
}

<<<<<<< HEAD
/* Must be called on unlocked socket. */
static void l2cap_sock_close(struct sock *sk)
{
	l2cap_sock_clear_timer(sk);
	lock_sock(sk);
	__l2cap_sock_close(sk, ECONNRESET);
	release_sock(sk);
	l2cap_sock_kill(sk);
}

static void l2cap_sock_cleanup_listen(struct sock *parent)
{
	struct sock *sk;

	BT_DBG("parent %p", parent);

	/* Close not yet accepted channels */
	while ((sk = bt_accept_dequeue(parent, NULL)))
		l2cap_sock_close(sk);

	parent->sk_state = BT_CLOSED;
	sock_set_flag(parent, SOCK_ZAPPED);
}

void __l2cap_sock_close(struct sock *sk, int reason)
{
	struct l2cap_conn *conn = l2cap_pi(sk)->conn;

	BT_DBG("sk %p state %d socket %p", sk, sk->sk_state, sk->sk_socket);

	switch (sk->sk_state) {
	case BT_LISTEN:
		l2cap_sock_cleanup_listen(sk);
		break;

	case BT_CONNECTED:
	case BT_CONFIG:
		if ((sk->sk_type == SOCK_SEQPACKET ||
					sk->sk_type == SOCK_STREAM) &&
					conn->hcon->type == ACL_LINK) {
			l2cap_sock_set_timer(sk, sk->sk_sndtimeo);
			l2cap_send_disconn_req(conn, sk, reason);
		} else
			l2cap_chan_del(sk, reason);
		break;

	case BT_CONNECT2:
		if ((sk->sk_type == SOCK_SEQPACKET ||
					sk->sk_type == SOCK_STREAM) &&
					conn->hcon->type == ACL_LINK) {
			struct l2cap_conn_rsp rsp;
			__u16 result;

			if (bt_sk(sk)->defer_setup)
				result = L2CAP_CR_SEC_BLOCK;
			else
				result = L2CAP_CR_BAD_PSM;
			sk->sk_state = BT_DISCONN;

			rsp.scid   = cpu_to_le16(l2cap_pi(sk)->dcid);
			rsp.dcid   = cpu_to_le16(l2cap_pi(sk)->scid);
			rsp.result = cpu_to_le16(result);
			rsp.status = cpu_to_le16(L2CAP_CS_NO_INFO);
			l2cap_send_cmd(conn, l2cap_pi(sk)->ident,
					L2CAP_CONN_RSP, sizeof(rsp), &rsp);
		}

		l2cap_chan_del(sk, reason);
		break;

	case BT_CONNECT:
	case BT_DISCONN:
		l2cap_chan_del(sk, reason);
		break;

	default:
		sock_set_flag(sk, SOCK_ZAPPED);
		break;
	}
=======
static int __l2cap_wait_ack(struct sock *sk, struct l2cap_chan *chan)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;
	int timeo = L2CAP_WAIT_ACK_POLL_PERIOD;
	/* Timeout to prevent infinite loop */
	unsigned long timeout = jiffies + L2CAP_WAIT_ACK_TIMEOUT;

	add_wait_queue(sk_sleep(sk), &wait);
	set_current_state(TASK_INTERRUPTIBLE);
	do {
		BT_DBG("Waiting for %d ACKs, timeout %04d ms",
		       chan->unacked_frames, time_after(jiffies, timeout) ? 0 :
		       jiffies_to_msecs(timeout - jiffies));

		if (!timeo)
			timeo = L2CAP_WAIT_ACK_POLL_PERIOD;

		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			break;
		}

		release_sock(sk);
		timeo = schedule_timeout(timeo);
		lock_sock(sk);
		set_current_state(TASK_INTERRUPTIBLE);

		err = sock_error(sk);
		if (err)
			break;

		if (time_after(jiffies, timeout)) {
			err = -ENOLINK;
			break;
		}

	} while (chan->unacked_frames > 0 &&
		 chan->state == BT_CONNECTED);

	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(sk), &wait);
	return err;
>>>>>>> android-4.9
}

static int l2cap_sock_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (!sk)
		return 0;

	lock_sock(sk);
<<<<<<< HEAD
	if (!sk->sk_shutdown) {

		if (l2cap_pi(sk)->mode == L2CAP_MODE_ERTM) {
			err = __l2cap_wait_ack(sk);
			l2cap_ertm_shutdown(sk);
		}

		sk->sk_shutdown = SHUTDOWN_MASK;
		l2cap_sock_clear_timer(sk);
		__l2cap_sock_close(sk, 0);
=======

	if (sk->sk_shutdown)
		goto shutdown_already;

	BT_DBG("Handling sock shutdown");

	/* prevent sk structure from being freed whilst unlocked */
	sock_hold(sk);

	chan = l2cap_pi(sk)->chan;
	/* prevent chan structure from being freed whilst unlocked */
	l2cap_chan_hold(chan);

	BT_DBG("chan %p state %s", chan, state_to_string(chan->state));

	if (chan->mode == L2CAP_MODE_ERTM &&
	    chan->unacked_frames > 0 &&
	    chan->state == BT_CONNECTED) {
		err = __l2cap_wait_ack(sk, chan);

		/* After waiting for ACKs, check whether shutdown
		 * has already been actioned to close the L2CAP
		 * link such as by l2cap_disconnection_req().
		 */
		if (sk->sk_shutdown)
			goto has_shutdown;
	}

	sk->sk_shutdown = SHUTDOWN_MASK;
	release_sock(sk);

	l2cap_chan_lock(chan);
	conn = chan->conn;
	if (conn)
		/* prevent conn structure from being freed */
		l2cap_conn_get(conn);
	l2cap_chan_unlock(chan);

	if (conn)
		/* mutex lock must be taken before l2cap_chan_lock() */
		mutex_lock(&conn->chan_lock);

	l2cap_chan_lock(chan);
	l2cap_chan_close(chan, 0);
	l2cap_chan_unlock(chan);

	if (conn) {
		mutex_unlock(&conn->chan_lock);
		l2cap_conn_put(conn);
	}

	lock_sock(sk);

	if (sock_flag(sk, SOCK_LINGER) && sk->sk_lingertime &&
	    !(current->flags & PF_EXITING))
		err = bt_sock_wait_state(sk, BT_CLOSED,
					 sk->sk_lingertime);
>>>>>>> android-4.9

has_shutdown:
	l2cap_chan_put(chan);
	sock_put(sk);

shutdown_already:
	if (!err && sk->sk_err)
		err = -sk->sk_err;

	release_sock(sk);
<<<<<<< HEAD
=======

	BT_DBG("Sock shutdown complete err: %d", err);

>>>>>>> android-4.9
	return err;
}

static int l2cap_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct sock *sk2 = NULL;
	int err;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (!sk)
		return 0;

<<<<<<< HEAD
	/* If this is an ATT socket, find it's matching server/client */
	if (l2cap_pi(sk)->scid == L2CAP_CID_LE_DATA)
		sk2 = l2cap_find_sock_by_fixed_cid_and_dir(L2CAP_CID_LE_DATA,
					&bt_sk(sk)->src, &bt_sk(sk)->dst,
					l2cap_pi(sk)->incoming ? 0 : 1);

	/* If matching socket found, request tear down */
	BT_DBG("sock:%p companion:%p", sk, sk2);
	if (sk2)
		l2cap_sock_set_timer(sk2, 1);
=======
	bt_sock_unlink(&l2cap_sk_list, sk);
>>>>>>> android-4.9

	err = l2cap_sock_shutdown(sock, 2);

	sock_orphan(sk);
	l2cap_sock_kill(sk);
	return err;
}

<<<<<<< HEAD
=======
static void l2cap_sock_cleanup_listen(struct sock *parent)
{
	struct sock *sk;

	BT_DBG("parent %p state %s", parent,
	       state_to_string(parent->sk_state));

	/* Close not yet accepted channels */
	while ((sk = bt_accept_dequeue(parent, NULL))) {
		struct l2cap_chan *chan = l2cap_pi(sk)->chan;

		BT_DBG("child chan %p state %s", chan,
		       state_to_string(chan->state));

		l2cap_chan_lock(chan);
		__clear_chan_timer(chan);
		l2cap_chan_close(chan, ECONNRESET);
		l2cap_chan_unlock(chan);

		l2cap_sock_kill(sk);
	}
}

static struct l2cap_chan *l2cap_sock_new_connection_cb(struct l2cap_chan *chan)
{
	struct sock *sk, *parent = chan->data;

	lock_sock(parent);

	/* Check for backlog size */
	if (sk_acceptq_is_full(parent)) {
		BT_DBG("backlog full %d", parent->sk_ack_backlog);
		release_sock(parent);
		return NULL;
	}

	sk = l2cap_sock_alloc(sock_net(parent), NULL, BTPROTO_L2CAP,
			      GFP_ATOMIC, 0);
	if (!sk) {
		release_sock(parent);
		return NULL;
        }

	bt_sock_reclassify_lock(sk, BTPROTO_L2CAP);

	l2cap_sock_init(sk, parent);

	bt_accept_enqueue(parent, sk);

	release_sock(parent);

	return l2cap_pi(sk)->chan;
}

static int l2cap_sock_recv_cb(struct l2cap_chan *chan, struct sk_buff *skb)
{
	struct sock *sk = chan->data;
	int err;

	lock_sock(sk);

	if (l2cap_pi(sk)->rx_busy_skb) {
		err = -ENOMEM;
		goto done;
	}

	if (chan->mode != L2CAP_MODE_ERTM &&
	    chan->mode != L2CAP_MODE_STREAMING) {
		/* Even if no filter is attached, we could potentially
		 * get errors from security modules, etc.
		 */
		err = sk_filter(sk, skb);
		if (err)
			goto done;
	}

	err = __sock_queue_rcv_skb(sk, skb);

	/* For ERTM, handle one skb that doesn't fit into the recv
	 * buffer.  This is important to do because the data frames
	 * have already been acked, so the skb cannot be discarded.
	 *
	 * Notify the l2cap core that the buffer is full, so the
	 * LOCAL_BUSY state is entered and no more frames are
	 * acked and reassembled until there is buffer space
	 * available.
	 */
	if (err < 0 && chan->mode == L2CAP_MODE_ERTM) {
		l2cap_pi(sk)->rx_busy_skb = skb;
		l2cap_chan_busy(chan, 1);
		err = 0;
	}

done:
	release_sock(sk);

	return err;
}

static void l2cap_sock_close_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;

	l2cap_sock_kill(sk);
}

static void l2cap_sock_teardown_cb(struct l2cap_chan *chan, int err)
{
	struct sock *sk = chan->data;
	struct sock *parent;

	BT_DBG("chan %p state %s", chan, state_to_string(chan->state));

	/* This callback can be called both for server (BT_LISTEN)
	 * sockets as well as "normal" ones. To avoid lockdep warnings
	 * with child socket locking (through l2cap_sock_cleanup_listen)
	 * we need separation into separate nesting levels. The simplest
	 * way to accomplish this is to inherit the nesting level used
	 * for the channel.
	 */
	lock_sock_nested(sk, atomic_read(&chan->nesting));

	parent = bt_sk(sk)->parent;

	sock_set_flag(sk, SOCK_ZAPPED);

	switch (chan->state) {
	case BT_OPEN:
	case BT_BOUND:
	case BT_CLOSED:
		break;
	case BT_LISTEN:
		l2cap_sock_cleanup_listen(sk);
		sk->sk_state = BT_CLOSED;
		chan->state = BT_CLOSED;

		break;
	default:
		sk->sk_state = BT_CLOSED;
		chan->state = BT_CLOSED;

		sk->sk_err = err;

		if (parent) {
			bt_accept_unlink(sk);
			parent->sk_data_ready(parent);
		} else {
			sk->sk_state_change(sk);
		}

		break;
	}

	release_sock(sk);
}

static void l2cap_sock_state_change_cb(struct l2cap_chan *chan, int state,
				       int err)
{
	struct sock *sk = chan->data;

	sk->sk_state = state;

	if (err)
		sk->sk_err = err;
}

static struct sk_buff *l2cap_sock_alloc_skb_cb(struct l2cap_chan *chan,
					       unsigned long hdr_len,
					       unsigned long len, int nb)
{
	struct sock *sk = chan->data;
	struct sk_buff *skb;
	int err;

	l2cap_chan_unlock(chan);
	skb = bt_skb_send_alloc(sk, hdr_len + len, nb, &err);
	l2cap_chan_lock(chan);

	if (!skb)
		return ERR_PTR(err);

	skb->priority = sk->sk_priority;

	bt_cb(skb)->l2cap.chan = chan;

	return skb;
}

static void l2cap_sock_ready_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;
	struct sock *parent;

	lock_sock(sk);

	parent = bt_sk(sk)->parent;

	BT_DBG("sk %p, parent %p", sk, parent);

	sk->sk_state = BT_CONNECTED;
	sk->sk_state_change(sk);

	if (parent)
		parent->sk_data_ready(parent);

	release_sock(sk);
}

static void l2cap_sock_defer_cb(struct l2cap_chan *chan)
{
	struct sock *parent, *sk = chan->data;

	lock_sock(sk);

	parent = bt_sk(sk)->parent;
	if (parent)
		parent->sk_data_ready(parent);

	release_sock(sk);
}

static void l2cap_sock_resume_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;

	if (test_and_clear_bit(FLAG_PENDING_SECURITY, &chan->flags)) {
		sk->sk_state = BT_CONNECTED;
		chan->state = BT_CONNECTED;
	}

	clear_bit(BT_SK_SUSPEND, &bt_sk(sk)->flags);
	sk->sk_state_change(sk);
}

static void l2cap_sock_set_shutdown_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;

	lock_sock(sk);
	sk->sk_shutdown = SHUTDOWN_MASK;
	release_sock(sk);
}

static long l2cap_sock_get_sndtimeo_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;

	return sk->sk_sndtimeo;
}

static void l2cap_sock_suspend_cb(struct l2cap_chan *chan)
{
	struct sock *sk = chan->data;

	set_bit(BT_SK_SUSPEND, &bt_sk(sk)->flags);
	sk->sk_state_change(sk);
}

static const struct l2cap_ops l2cap_chan_ops = {
	.name			= "L2CAP Socket Interface",
	.new_connection		= l2cap_sock_new_connection_cb,
	.recv			= l2cap_sock_recv_cb,
	.close			= l2cap_sock_close_cb,
	.teardown		= l2cap_sock_teardown_cb,
	.state_change		= l2cap_sock_state_change_cb,
	.ready			= l2cap_sock_ready_cb,
	.defer			= l2cap_sock_defer_cb,
	.resume			= l2cap_sock_resume_cb,
	.suspend		= l2cap_sock_suspend_cb,
	.set_shutdown		= l2cap_sock_set_shutdown_cb,
	.get_sndtimeo		= l2cap_sock_get_sndtimeo_cb,
	.alloc_skb		= l2cap_sock_alloc_skb_cb,
};

>>>>>>> android-4.9
static void l2cap_sock_destruct(struct sock *sk)
{
	BT_DBG("sk %p", sk);

<<<<<<< HEAD
=======
	if (l2cap_pi(sk)->chan)
		l2cap_chan_put(l2cap_pi(sk)->chan);

	if (l2cap_pi(sk)->rx_busy_skb) {
		kfree_skb(l2cap_pi(sk)->rx_busy_skb);
		l2cap_pi(sk)->rx_busy_skb = NULL;
	}

>>>>>>> android-4.9
	skb_queue_purge(&sk->sk_receive_queue);
	skb_queue_purge(&sk->sk_write_queue);

	l2cap_ertm_destruct(sk);
}

<<<<<<< HEAD
static void set_default_config(struct l2cap_conf_prm *conf_prm)
{
	conf_prm->fcs = L2CAP_FCS_CRC16;
	conf_prm->flush_to = L2CAP_DEFAULT_FLUSH_TO;
}

void l2cap_sock_init(struct sock *sk, struct sock *parent)
{
	struct l2cap_pinfo *pi = l2cap_pi(sk);
=======
static void l2cap_skb_msg_name(struct sk_buff *skb, void *msg_name,
			       int *msg_namelen)
{
	DECLARE_SOCKADDR(struct sockaddr_l2 *, la, msg_name);

	memset(la, 0, sizeof(struct sockaddr_l2));
	la->l2_family = AF_BLUETOOTH;
	la->l2_psm = bt_cb(skb)->l2cap.psm;
	bacpy(&la->l2_bdaddr, &bt_cb(skb)->l2cap.bdaddr);

	*msg_namelen = sizeof(struct sockaddr_l2);
}

static void l2cap_sock_init(struct sock *sk, struct sock *parent)
{
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
>>>>>>> android-4.9

	BT_DBG("sk %p parent %p", sk, parent);

	if (parent) {
		sk->sk_type = parent->sk_type;
<<<<<<< HEAD
		sk->sk_rcvbuf = parent->sk_rcvbuf;
		sk->sk_sndbuf = parent->sk_sndbuf;
		bt_sk(sk)->defer_setup = bt_sk(parent)->defer_setup;

		pi->imtu = l2cap_pi(parent)->imtu;
		pi->omtu = l2cap_pi(parent)->omtu;
		pi->conf_state = l2cap_pi(parent)->conf_state;
		pi->mode = l2cap_pi(parent)->mode;
		pi->fcs  = l2cap_pi(parent)->fcs;
		pi->max_tx = l2cap_pi(parent)->max_tx;
		pi->tx_win = l2cap_pi(parent)->tx_win;
		pi->sec_level = l2cap_pi(parent)->sec_level;
		pi->role_switch = l2cap_pi(parent)->role_switch;
		pi->force_reliable = l2cap_pi(parent)->force_reliable;
		pi->flushable = l2cap_pi(parent)->flushable;
		pi->force_active = l2cap_pi(parent)->force_active;
		pi->amp_pref = l2cap_pi(parent)->amp_pref;
	} else {
		pi->imtu = L2CAP_DEFAULT_MTU;
		pi->omtu = 0;
=======
		bt_sk(sk)->flags = bt_sk(parent)->flags;

		chan->chan_type = pchan->chan_type;
		chan->imtu = pchan->imtu;
		chan->omtu = pchan->omtu;
		chan->conf_state = pchan->conf_state;
		chan->mode = pchan->mode;
		chan->fcs  = pchan->fcs;
		chan->max_tx = pchan->max_tx;
		chan->tx_win = pchan->tx_win;
		chan->tx_win_max = pchan->tx_win_max;
		chan->sec_level = pchan->sec_level;
		chan->flags = pchan->flags;
		chan->tx_credits = pchan->tx_credits;
		chan->rx_credits = pchan->rx_credits;

		if (chan->chan_type == L2CAP_CHAN_FIXED) {
			chan->scid = pchan->scid;
			chan->dcid = pchan->scid;
		}

		security_sk_clone(parent, sk);
	} else {
		switch (sk->sk_type) {
		case SOCK_RAW:
			chan->chan_type = L2CAP_CHAN_RAW;
			break;
		case SOCK_DGRAM:
			chan->chan_type = L2CAP_CHAN_CONN_LESS;
			bt_sk(sk)->skb_msg_name = l2cap_skb_msg_name;
			break;
		case SOCK_SEQPACKET:
		case SOCK_STREAM:
			chan->chan_type = L2CAP_CHAN_CONN_ORIENTED;
			break;
		}

		chan->imtu = L2CAP_DEFAULT_MTU;
		chan->omtu = 0;
>>>>>>> android-4.9
		if (!disable_ertm && sk->sk_type == SOCK_STREAM) {
			pi->mode = L2CAP_MODE_ERTM;
			pi->conf_state |= L2CAP_CONF_STATE2_DEVICE;
		} else {
			pi->mode = L2CAP_MODE_BASIC;
		}
<<<<<<< HEAD
		pi->reconf_state = L2CAP_RECONF_NONE;
		pi->max_tx = L2CAP_DEFAULT_MAX_TX;
		pi->fcs = L2CAP_FCS_CRC16;
		pi->tx_win = L2CAP_DEFAULT_TX_WINDOW;
		pi->sec_level = BT_SECURITY_LOW;
		pi->role_switch = 0;
		pi->force_reliable = 0;
		pi->flushable = 0;
		pi->force_active = 1;
		pi->amp_pref = BT_AMP_POLICY_REQUIRE_BR_EDR;
=======

		l2cap_chan_set_defaults(chan);
>>>>>>> android-4.9
	}

	/* Default config options */
	sk->sk_backlog_rcv = l2cap_data_channel;
	pi->ampcon = NULL;
	pi->ampchan = NULL;
	pi->conf_len = 0;
	pi->flush_to = L2CAP_DEFAULT_FLUSH_TO;
	pi->scid = 0;
	pi->dcid = 0;
	pi->tx_win_max = L2CAP_TX_WIN_MAX_ENHANCED;
	pi->ack_win = pi->tx_win;
	pi->extended_control = 0;

	pi->local_conf.fcs = pi->fcs;
	pi->local_conf.flush_to = pi->flush_to;

	set_default_config(&pi->remote_conf);

	skb_queue_head_init(TX_QUEUE(sk));
	skb_queue_head_init(SREJ_QUEUE(sk));
}

static struct proto l2cap_proto = {
	.name		= "L2CAP",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct l2cap_pinfo)
};

<<<<<<< HEAD
struct sock *l2cap_sock_alloc(struct net *net, struct socket *sock, int proto, gfp_t prio)
=======
static struct sock *l2cap_sock_alloc(struct net *net, struct socket *sock,
				     int proto, gfp_t prio, int kern)
>>>>>>> android-4.9
{
	struct sock *sk;

	sk = sk_alloc(net, PF_BLUETOOTH, prio, &l2cap_proto, kern);
	if (!sk)
		return NULL;

	sock_init_data(sock, sk);
	INIT_LIST_HEAD(&bt_sk(sk)->accept_q);

	sk->sk_destruct = l2cap_sock_destruct;
	sk->sk_sndtimeo = msecs_to_jiffies(L2CAP_CONN_TIMEOUT);

	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = proto;
	sk->sk_state = BT_OPEN;

<<<<<<< HEAD
	setup_timer(&sk->sk_timer, l2cap_sock_timeout, (unsigned long) sk);
=======
	chan = l2cap_chan_create();
	if (!chan) {
		sk_free(sk);
		return NULL;
	}

	l2cap_chan_hold(chan);

	l2cap_pi(sk)->chan = chan;
>>>>>>> android-4.9

	bt_sock_link(&l2cap_sk_list, sk);
	return sk;
}

static int l2cap_sock_create(struct net *net, struct socket *sock, int protocol,
			     int kern)
{
	struct sock *sk;

	BT_DBG("sock %p", sock);

	sock->state = SS_UNCONNECTED;

	if (sock->type != SOCK_SEQPACKET && sock->type != SOCK_STREAM &&
	    sock->type != SOCK_DGRAM && sock->type != SOCK_RAW)
		return -ESOCKTNOSUPPORT;

	if (sock->type == SOCK_RAW && !kern && !capable(CAP_NET_RAW))
		return -EPERM;

	sock->ops = &l2cap_sock_ops;

	sk = l2cap_sock_alloc(net, sock, protocol, GFP_ATOMIC, kern);
	if (!sk)
		return -ENOMEM;

	l2cap_sock_init(sk, NULL);
	bt_sock_link(&l2cap_sk_list, sk);
	return 0;
}

const struct proto_ops l2cap_sock_ops = {
	.family		= PF_BLUETOOTH,
	.owner		= THIS_MODULE,
	.release	= l2cap_sock_release,
	.bind		= l2cap_sock_bind,
	.connect	= l2cap_sock_connect,
	.listen		= l2cap_sock_listen,
	.accept		= l2cap_sock_accept,
	.getname	= l2cap_sock_getname,
	.sendmsg	= l2cap_sock_sendmsg,
	.recvmsg	= l2cap_sock_recvmsg,
	.poll		= bt_sock_poll,
	.ioctl		= bt_sock_ioctl,
	.mmap		= sock_no_mmap,
	.socketpair	= sock_no_socketpair,
	.shutdown	= l2cap_sock_shutdown,
	.setsockopt	= l2cap_sock_setsockopt,
	.getsockopt	= l2cap_sock_getsockopt
};

static const struct net_proto_family l2cap_sock_family_ops = {
	.family	= PF_BLUETOOTH,
	.owner	= THIS_MODULE,
	.create	= l2cap_sock_create,
};

int __init l2cap_init_sockets(void)
{
	int err;

	BUILD_BUG_ON(sizeof(struct sockaddr_l2) > sizeof(struct sockaddr));

	err = proto_register(&l2cap_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_L2CAP, &l2cap_sock_family_ops);
	if (err < 0) {
		BT_ERR("L2CAP socket registration failed");
		goto error;
	}

	err = bt_procfs_init(&init_net, "l2cap", &l2cap_sk_list,
			     NULL);
	if (err < 0) {
		BT_ERR("Failed to create L2CAP proc file");
		bt_sock_unregister(BTPROTO_L2CAP);
		goto error;
	}

	BT_INFO("L2CAP socket layer initialized");

	return 0;

error:
	proto_unregister(&l2cap_proto);
	return err;
}

void l2cap_cleanup_sockets(void)
{
	bt_procfs_cleanup(&init_net, "l2cap");
	bt_sock_unregister(BTPROTO_L2CAP);
	proto_unregister(&l2cap_proto);
}
