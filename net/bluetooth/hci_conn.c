/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (c) 2000-2001, 2010-2012 The Linux Foundation.  All rights reserved.

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

/* Bluetooth HCI connection handling. */

<<<<<<< HEAD
#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <linux/notifier.h>
#include <net/sock.h>

#include <asm/system.h>
#include <linux/uaccess.h>
#include <asm/unaligned.h>
=======
#include <linux/export.h>
#include <linux/debugfs.h>
>>>>>>> android-4.9

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/l2cap.h>
<<<<<<< HEAD

struct hci_conn *hci_le_connect(struct hci_dev *hdev, __u16 pkt_type,
				bdaddr_t *dst, __u8 sec_level, __u8 auth_type,
				struct bt_le_params *le_params)
{
	struct hci_conn *le, *le_wlist_conn;
	struct hci_cp_le_create_conn cp;
	struct adv_entry *entry;
	struct link_key *key;

	BT_DBG("%p", hdev);

	le = hci_conn_hash_lookup_ba(hdev, LE_LINK, dst);
	if (le) {
		le_wlist_conn = hci_conn_hash_lookup_ba(hdev, LE_LINK,
								BDADDR_ANY);
		if (!le_wlist_conn) {
			hci_conn_hold(le);
			return le;
		} else {
			BT_DBG("remove wlist conn");
			le->out = 1;
			le->link_mode |= HCI_LM_MASTER;
			le->sec_level = BT_SECURITY_LOW;
			le->type = LE_LINK;
			hci_proto_connect_cfm(le, 0);
			hci_conn_del(le_wlist_conn);
			return le;
		}
	}

	key = hci_find_link_key_type(hdev, dst, KEY_TYPE_LTK);
	if (!key) {
		entry = hci_find_adv_entry(hdev, dst);
		if (entry)
			le = hci_le_conn_add(hdev, dst,
					entry->bdaddr_type);
		else
			le = hci_le_conn_add(hdev, dst, 0);
	} else {
		le = hci_le_conn_add(hdev, dst, key->addr_type);
	}

	if (!le)
		return ERR_PTR(-ENOMEM);

	hci_conn_hold(le);

	le->state = BT_CONNECT;
	le->out = 1;
	le->link_mode |= HCI_LM_MASTER;
	le->sec_level = BT_SECURITY_LOW;
	le->type = LE_LINK;

	memset(&cp, 0, sizeof(cp));
	if (l2cap_sock_le_params_valid(le_params)) {
		cp.supervision_timeout =
				cpu_to_le16(le_params->supervision_timeout);
		cp.scan_interval = cpu_to_le16(le_params->scan_interval);
		cp.scan_window = cpu_to_le16(le_params->scan_window);
		cp.conn_interval_min = cpu_to_le16(le_params->interval_min);
		cp.conn_interval_max = cpu_to_le16(le_params->interval_max);
		cp.conn_latency = cpu_to_le16(le_params->latency);
		cp.min_ce_len = cpu_to_le16(le_params->min_ce_len);
		cp.max_ce_len = cpu_to_le16(le_params->max_ce_len);
		le->conn_timeout = le_params->conn_timeout;
	} else {
		cp.supervision_timeout = cpu_to_le16(BT_LE_SUP_TO_DEFAULT);
		cp.scan_interval = cpu_to_le16(BT_LE_SCAN_INTERVAL_DEF);
		cp.scan_window = cpu_to_le16(BT_LE_SCAN_WINDOW_DEF);
		cp.conn_interval_min = cpu_to_le16(BT_LE_CONN_INTERVAL_MIN_DEF);
		cp.conn_interval_max = cpu_to_le16(BT_LE_CONN_INTERVAL_MAX_DEF);
		cp.conn_latency = cpu_to_le16(BT_LE_LATENCY_DEF);
		le->conn_timeout = 5;
	}
	if (!bacmp(&le->dst, BDADDR_ANY)) {
		cp.filter_policy = 0x01;
		le->conn_timeout = 0;
	} else {
		bacpy(&cp.peer_addr, &le->dst);
		cp.peer_addr_type = le->dst_type;
	}

	hci_send_cmd(hdev, HCI_OP_LE_CREATE_CONN, sizeof(cp), &cp);

	return le;
=======

#include "hci_request.h"
#include "smp.h"
#include "a2mp.h"

struct sco_param {
	u16 pkt_type;
	u16 max_latency;
	u8  retrans_effort;
};

static const struct sco_param esco_param_cvsd[] = {
	{ EDR_ESCO_MASK & ~ESCO_2EV3, 0x000a,	0x01 }, /* S3 */
	{ EDR_ESCO_MASK & ~ESCO_2EV3, 0x0007,	0x01 }, /* S2 */
	{ EDR_ESCO_MASK | ESCO_EV3,   0x0007,	0x01 }, /* S1 */
	{ EDR_ESCO_MASK | ESCO_HV3,   0xffff,	0x01 }, /* D1 */
	{ EDR_ESCO_MASK | ESCO_HV1,   0xffff,	0x01 }, /* D0 */
};

static const struct sco_param sco_param_cvsd[] = {
	{ EDR_ESCO_MASK | ESCO_HV3,   0xffff,	0xff }, /* D1 */
	{ EDR_ESCO_MASK | ESCO_HV1,   0xffff,	0xff }, /* D0 */
};

static const struct sco_param esco_param_msbc[] = {
	{ EDR_ESCO_MASK & ~ESCO_2EV3, 0x000d,	0x02 }, /* T2 */
	{ EDR_ESCO_MASK | ESCO_EV3,   0x0008,	0x02 }, /* T1 */
};

/* This function requires the caller holds hdev->lock */
static void hci_connect_le_scan_cleanup(struct hci_conn *conn)
{
	struct hci_conn_params *params;
	struct hci_dev *hdev = conn->hdev;
	struct smp_irk *irk;
	bdaddr_t *bdaddr;
	u8 bdaddr_type;

	bdaddr = &conn->dst;
	bdaddr_type = conn->dst_type;

	/* Check if we need to convert to identity address */
	irk = hci_get_irk(hdev, bdaddr, bdaddr_type);
	if (irk) {
		bdaddr = &irk->bdaddr;
		bdaddr_type = irk->addr_type;
	}

	params = hci_pend_le_action_lookup(&hdev->pend_le_conns, bdaddr,
					   bdaddr_type);
	if (!params || !params->explicit_connect)
		return;

	/* The connection attempt was doing scan for new RPA, and is
	 * in scan phase. If params are not associated with any other
	 * autoconnect action, remove them completely. If they are, just unmark
	 * them as waiting for connection, by clearing explicit_connect field.
	 */
	params->explicit_connect = false;

	list_del_init(&params->action);

	switch (params->auto_connect) {
	case HCI_AUTO_CONN_EXPLICIT:
		hci_conn_params_del(hdev, bdaddr, bdaddr_type);
		/* return instead of break to avoid duplicate scan update */
		return;
	case HCI_AUTO_CONN_DIRECT:
	case HCI_AUTO_CONN_ALWAYS:
		list_add(&params->action, &hdev->pend_le_conns);
		break;
	case HCI_AUTO_CONN_REPORT:
		list_add(&params->action, &hdev->pend_le_reports);
		break;
	default:
		break;
	}

	hci_update_background_scan(hdev);
}

static void hci_conn_cleanup(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

	if (test_bit(HCI_CONN_PARAM_REMOVAL_PEND, &conn->flags))
		hci_conn_params_del(conn->hdev, &conn->dst, conn->dst_type);

	hci_chan_list_flush(conn);

	hci_conn_hash_del(hdev, conn);

	if (hdev->notify)
		hdev->notify(hdev, HCI_NOTIFY_CONN_DEL);

	hci_conn_del_sysfs(conn);

	debugfs_remove_recursive(conn->debugfs);

	hci_dev_put(hdev);

	hci_conn_put(conn);
>>>>>>> android-4.9
}
EXPORT_SYMBOL(hci_le_connect);

static void le_scan_cleanup(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     le_scan_cleanup);
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn *c = NULL;

	BT_DBG("%s hcon %p", hdev->name, conn);

	hci_dev_lock(hdev);

	/* Check that the hci_conn is still around */
	rcu_read_lock();
	list_for_each_entry_rcu(c, &hdev->conn_hash.list, list) {
		if (c == conn)
			break;
	}
	rcu_read_unlock();

	if (c == conn) {
		hci_connect_le_scan_cleanup(conn);
		hci_conn_cleanup(conn);
	}

	hci_dev_unlock(hdev);
	hci_dev_put(hdev);
	hci_conn_put(conn);
}

<<<<<<< HEAD
void hci_le_cancel_create_connect(struct hci_dev *hdev, bdaddr_t *dst)
{
	struct hci_conn *le;

	BT_DBG("%p", hdev);

	le = hci_conn_hash_lookup_ba(hdev, LE_LINK, dst);
	if (le) {
		BT_DBG("send hci connect cancel");
		hci_le_connect_cancel(le);
		hci_conn_del(le);
	}
}
EXPORT_SYMBOL(hci_le_cancel_create_connect);

void hci_le_add_dev_white_list(struct hci_dev *hdev, bdaddr_t *dst)
{
	struct hci_cp_le_add_dev_white_list cp;
	struct adv_entry *entry;
	struct link_key *key;

	BT_DBG("%p", hdev);

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.addr, dst);

	key = hci_find_link_key_type(hdev, dst, KEY_TYPE_LTK);
	if (!key) {
		entry = hci_find_adv_entry(hdev, dst);
		if (entry)
			cp.addr_type = entry->bdaddr_type;
		else
			cp.addr_type = 0x00;
	} else {
		cp.addr_type = key->addr_type;
	}

	hci_send_cmd(hdev, HCI_OP_LE_ADD_DEV_WHITE_LIST, sizeof(cp), &cp);
}
EXPORT_SYMBOL(hci_le_add_dev_white_list);

void hci_le_remove_dev_white_list(struct hci_dev *hdev, bdaddr_t *dst)
{
	struct hci_cp_le_remove_dev_white_list cp;
	struct adv_entry *entry;
	struct link_key *key;

	BT_DBG("%p", hdev);

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.addr, dst);

	key = hci_find_link_key_type(hdev, dst, KEY_TYPE_LTK);
	if (!key) {
		entry = hci_find_adv_entry(hdev, dst);
		if (entry)
			cp.addr_type = entry->bdaddr_type;
		else
			cp.addr_type = 0x00;
	} else {
		cp.addr_type = key->addr_type;
	}

	hci_send_cmd(hdev, HCI_OP_LE_REMOVE_DEV_WHITE_LIST, sizeof(cp), &cp);
}
EXPORT_SYMBOL(hci_le_remove_dev_white_list);

static inline bool is_role_switch_possible(struct hci_dev *hdev)
{
	if (hci_conn_hash_lookup_state(hdev, ACL_LINK, BT_CONNECTED))
		return false;
	return true;
}

void hci_acl_connect(struct hci_conn *conn)
=======
static void hci_connect_le_scan_remove(struct hci_conn *conn)
{
	BT_DBG("%s hcon %p", conn->hdev->name, conn);

	/* We can't call hci_conn_del/hci_conn_cleanup here since that
	 * could deadlock with another hci_conn_del() call that's holding
	 * hci_dev_lock and doing cancel_delayed_work_sync(&conn->disc_work).
	 * Instead, grab temporary extra references to the hci_dev and
	 * hci_conn and perform the necessary cleanup in a separate work
	 * callback.
	 */

	hci_dev_hold(conn->hdev);
	hci_conn_get(conn);

	/* Even though we hold a reference to the hdev, many other
	 * things might get cleaned up meanwhile, including the hdev's
	 * own workqueue, so we can't use that for scheduling.
	 */
	schedule_work(&conn->le_scan_cleanup);
}

static void hci_acl_create_connection(struct hci_conn *conn)
>>>>>>> android-4.9
{
	struct hci_dev *hdev = conn->hdev;
	struct inquiry_entry *ie;
	struct hci_cp_create_conn cp;

	BT_DBG("%p", conn);

	conn->state = BT_CONNECT;
<<<<<<< HEAD
	conn->out = 1;

	conn->link_mode = HCI_LM_MASTER;
=======
	conn->out = true;
	conn->role = HCI_ROLE_MASTER;
>>>>>>> android-4.9

	conn->attempt++;

	conn->link_policy = hdev->link_policy;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, &conn->dst);
	cp.pscan_rep_mode = 0x02;

	ie = hci_inquiry_cache_lookup(hdev, &conn->dst);
	if (ie) {
		if (inquiry_entry_age(ie) <= INQUIRY_ENTRY_AGE_MAX) {
			cp.pscan_rep_mode = ie->data.pscan_rep_mode;
			cp.pscan_mode     = ie->data.pscan_mode;
			cp.clock_offset   = ie->data.clock_offset |
					    cpu_to_le16(0x8000);
		}

		memcpy(conn->dev_class, ie->data.dev_class, 3);
		conn->ssp_mode = ie->data.ssp_mode;
	}

	cp.pkt_type = cpu_to_le16(conn->pkt_type);
	if (lmp_rswitch_capable(hdev) && !(hdev->link_mode & HCI_LM_MASTER)
		&& is_role_switch_possible(hdev))
		cp.role_switch = 0x01;
	else
		cp.role_switch = 0x00;

	hci_send_cmd(hdev, HCI_OP_CREATE_CONN, sizeof(cp), &cp);
}

<<<<<<< HEAD
static void hci_acl_connect_cancel(struct hci_conn *conn)
{
	struct hci_cp_create_conn_cancel cp;

	BT_DBG("%p", conn);

	if (conn->hdev->hci_ver < 2)
		return;

	bacpy(&cp.bdaddr, &conn->dst);
	hci_send_cmd(conn->hdev, HCI_OP_CREATE_CONN_CANCEL, sizeof(cp), &cp);
}

void hci_acl_disconn(struct hci_conn *conn, __u8 reason)
{
	BT_DBG("%p", conn);
=======
int hci_disconnect(struct hci_conn *conn, __u8 reason)
{
	BT_DBG("hcon %p", conn);
>>>>>>> android-4.9

	/* When we are master of an established connection and it enters
	 * the disconnect timeout, then go ahead and try to read the
	 * current clock offset.  Processing of the result is done
	 * within the event handling and hci_clock_offset_evt function.
	 */
	if (conn->type == ACL_LINK && conn->role == HCI_ROLE_MASTER &&
	    (conn->state == BT_CONNECTED || conn->state == BT_CONFIG)) {
		struct hci_dev *hdev = conn->hdev;
		struct hci_cp_read_clock_offset clkoff_cp;

		clkoff_cp.handle = cpu_to_le16(conn->handle);
		hci_send_cmd(hdev, HCI_OP_READ_CLOCK_OFFSET, sizeof(clkoff_cp),
			     &clkoff_cp);
	}

<<<<<<< HEAD
	if (conn->hdev->dev_type == HCI_BREDR) {
		struct hci_cp_disconnect cp;
		cp.handle = cpu_to_le16(conn->handle);
		cp.reason = reason;
		hci_send_cmd(conn->hdev, HCI_OP_DISCONNECT, sizeof(cp), &cp);
	} else {
		struct hci_cp_disconn_phys_link cp;
		cp.phy_handle = (u8) conn->handle;
		cp.reason = reason;
		hci_send_cmd(conn->hdev, HCI_OP_DISCONN_PHYS_LINK,
				sizeof(cp), &cp);
	}
=======
	return hci_abort_conn(conn, reason);
>>>>>>> android-4.9
}

static void hci_add_sco(struct hci_conn *conn, __u16 handle)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_cp_add_sco cp;

	BT_DBG("hcon %p", conn);

	conn->state = BT_CONNECT;
	conn->out = 1;

	conn->attempt++;

	cp.handle   = cpu_to_le16(handle);
	cp.pkt_type = cpu_to_le16(conn->pkt_type);

	hci_send_cmd(hdev, HCI_OP_ADD_SCO, sizeof(cp), &cp);
}

bool hci_setup_sync(struct hci_conn *conn, __u16 handle)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_cp_setup_sync_conn cp;
	const struct sco_param *param;

	BT_DBG("hcon %p", conn);

	conn->state = BT_CONNECT;
	conn->out = 1;

	conn->attempt++;

	cp.handle   = cpu_to_le16(handle);

	cp.tx_bandwidth   = cpu_to_le32(0x00001f40);
	cp.rx_bandwidth   = cpu_to_le32(0x00001f40);
<<<<<<< HEAD
	if (conn->hdev->is_wbs) {
		/* Transparent Data */
		uint16_t voice_setting = hdev->voice_setting | ACF_TRANS;
		cp.max_latency    = cpu_to_le16(0x000D);
		cp.pkt_type = cpu_to_le16(ESCO_WBS);
		cp.voice_setting  = cpu_to_le16(voice_setting);
		/* Retransmission Effort */
		cp.retrans_effort = RE_LINK_QUALITY;
	} else {
		cp.max_latency    = cpu_to_le16(0x000A);
		cp.pkt_type = cpu_to_le16(conn->pkt_type);
		cp.voice_setting  = cpu_to_le16(hdev->voice_setting);
		cp.retrans_effort = RE_POWER_CONSUMP;
	}
=======
	cp.voice_setting  = cpu_to_le16(conn->setting);
>>>>>>> android-4.9

	switch (conn->setting & SCO_AIRMODE_MASK) {
	case SCO_AIRMODE_TRANSP:
		if (conn->attempt > ARRAY_SIZE(esco_param_msbc))
			return false;
		param = &esco_param_msbc[conn->attempt - 1];
		break;
	case SCO_AIRMODE_CVSD:
		if (lmp_esco_capable(conn->link)) {
			if (conn->attempt > ARRAY_SIZE(esco_param_cvsd))
				return false;
			param = &esco_param_cvsd[conn->attempt - 1];
		} else {
			if (conn->attempt > ARRAY_SIZE(sco_param_cvsd))
				return false;
			param = &sco_param_cvsd[conn->attempt - 1];
		}
		break;
	default:
		return false;
	}

	cp.retrans_effort = param->retrans_effort;
	cp.pkt_type = __cpu_to_le16(param->pkt_type);
	cp.max_latency = __cpu_to_le16(param->max_latency);

	if (hci_send_cmd(hdev, HCI_OP_SETUP_SYNC_CONN, sizeof(cp), &cp) < 0)
		return false;

	return true;
}

u8 hci_le_conn_update(struct hci_conn *conn, u16 min, u16 max, u16 latency,
		      u16 to_multiplier)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn_params *params;
	struct hci_cp_le_conn_update cp;

	hci_dev_lock(hdev);

	params = hci_conn_params_lookup(hdev, &conn->dst, conn->dst_type);
	if (params) {
		params->conn_min_interval = min;
		params->conn_max_interval = max;
		params->conn_latency = latency;
		params->supervision_timeout = to_multiplier;
	}

	hci_dev_unlock(hdev);

	memset(&cp, 0, sizeof(cp));
	cp.handle		= cpu_to_le16(conn->handle);
	cp.conn_interval_min	= cpu_to_le16(min);
	cp.conn_interval_max	= cpu_to_le16(max);
	cp.conn_latency		= cpu_to_le16(latency);
	cp.supervision_timeout	= cpu_to_le16(to_multiplier);
	cp.min_ce_len		= cpu_to_le16(0x0000);
	cp.max_ce_len		= cpu_to_le16(0x0000);

	hci_send_cmd(hdev, HCI_OP_LE_CONN_UPDATE, sizeof(cp), &cp);

	if (params)
		return 0x01;

	return 0x00;
}

<<<<<<< HEAD
void hci_read_rssi(struct hci_conn *conn)
{
	struct hci_cp_read_rssi cp;
	struct hci_dev *hdev = conn->hdev;

	memset(&cp, 0, sizeof(cp));
	cp.handle   = cpu_to_le16(conn->handle);

	hci_send_cmd(hdev, HCI_OP_READ_RSSI, sizeof(cp), &cp);
}
EXPORT_SYMBOL(hci_read_rssi);

void hci_le_start_enc(struct hci_conn *conn, __le16 ediv, __u8 rand[8],
							__u8 ltk[16])
=======
void hci_le_start_enc(struct hci_conn *conn, __le16 ediv, __le64 rand,
		      __u8 ltk[16], __u8 key_size)
>>>>>>> android-4.9
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_cp_le_start_enc cp;

	BT_DBG("hcon %p", conn);

	memset(&cp, 0, sizeof(cp));

	cp.handle = cpu_to_le16(conn->handle);
	cp.rand = rand;
	cp.ediv = ediv;
	memcpy(cp.ltk, ltk, key_size);

	hci_send_cmd(hdev, HCI_OP_LE_START_ENC, sizeof(cp), &cp);
}
<<<<<<< HEAD
EXPORT_SYMBOL(hci_le_start_enc);

void hci_le_ltk_reply(struct hci_conn *conn, __u8 ltk[16])
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_cp_le_ltk_reply cp;

	BT_DBG("%p", conn);

	memset(&cp, 0, sizeof(cp));

	cp.handle = cpu_to_le16(conn->handle);
	memcpy(cp.ltk, ltk, sizeof(cp.ltk));

	hci_send_cmd(hdev, HCI_OP_LE_LTK_REPLY, sizeof(cp), &cp);
}
EXPORT_SYMBOL(hci_le_ltk_reply);

void hci_le_ltk_neg_reply(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_cp_le_ltk_neg_reply cp;

	BT_DBG("%p", conn);

	memset(&cp, 0, sizeof(cp));

	cp.handle = cpu_to_le16(conn->handle);

	hci_send_cmd(hdev, HCI_OP_LE_LTK_NEG_REPLY, sizeof(cp), &cp);
}
=======
>>>>>>> android-4.9

/* Device _must_ be locked */
void hci_sco_setup(struct hci_conn *conn, __u8 status)
{
	struct hci_conn *sco = conn->link;

	if (!sco)
		return;

	BT_DBG("hcon %p", conn);

	if (!status) {
		if (lmp_esco_capable(conn->hdev))
			hci_setup_sync(sco, conn->handle);
		else
			hci_add_sco(sco, conn->handle);
	} else {
		hci_connect_cfm(sco, status);
		hci_conn_del(sco);
	}
}

static void hci_conn_timeout(unsigned long arg)
{
<<<<<<< HEAD
	struct hci_conn *conn = (void *) arg;
	struct hci_dev *hdev = conn->hdev;
	__u8 reason;

	BT_DBG("conn %p state %d", conn, conn->state);

	hci_dev_lock(hdev);

	switch (conn->state) {
	case BT_CONNECT:
	case BT_CONNECT2:
		if (conn->out) {
			if (conn->type == ACL_LINK)
				hci_acl_connect_cancel(conn);
			else if (conn->type == LE_LINK)
				hci_le_connect_cancel(conn);
		}
		break;
	case BT_CONFIG:
	case BT_CONNECTED:
		if (!atomic_read(&conn->refcnt)) {
			reason = hci_proto_disconn_ind(conn);
			hci_acl_disconn(conn, reason);
		}
		break;
	default:
		if (!atomic_read(&conn->refcnt))
			conn->state = BT_CLOSED;
		break;
	}

	hci_dev_unlock(hdev);
}

static void hci_conn_idle(unsigned long arg)
{
	struct hci_conn *conn = (void *) arg;

	BT_DBG("conn %p mode %d", conn, conn->mode);

	hci_conn_enter_sniff_mode(conn);
}

static void hci_conn_rssi_update(struct work_struct *work)
{
	struct delayed_work *delayed =
		container_of(work, struct delayed_work, work);
	struct hci_conn *conn =
		container_of(delayed, struct hci_conn, rssi_update_work);

	BT_DBG("conn %p mode %d", conn, conn->mode);

	hci_read_rssi(conn);
}

static void encryption_disabled_timeout(unsigned long userdata)
{
	struct hci_conn *conn = (struct hci_conn *)userdata;
	BT_INFO("conn %p Grace Prd Exp ", conn);

	hci_encrypt_cfm(conn, 0, 0);

	if (test_bit(HCI_CONN_ENCRYPT_PEND, &conn->pend)) {
		struct hci_cp_set_conn_encrypt cp;
		BT_INFO("HCI_CONN_ENCRYPT_PEND is set");
		cp.handle  = cpu_to_le16(conn->handle);
		cp.encrypt = 1;
		hci_send_cmd(conn->hdev, HCI_OP_SET_CONN_ENCRYPT,
						sizeof(cp), &cp);
	}

}

struct hci_conn *hci_conn_add(struct hci_dev *hdev, int type,
					__u16 pkt_type, bdaddr_t *dst)
=======
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     disc_work.work);
	int refcnt = atomic_read(&conn->refcnt);

	BT_DBG("hcon %p state %s", conn, state_to_string(conn->state));

	WARN_ON(refcnt < 0);

	/* FIXME: It was observed that in pairing failed scenario, refcnt
	 * drops below 0. Probably this is because l2cap_conn_del calls
	 * l2cap_chan_del for each channel, and inside l2cap_chan_del conn is
	 * dropped. After that loop hci_chan_del is called which also drops
	 * conn. For now make sure that ACL is alive if refcnt is higher then 0,
	 * otherwise drop it.
	 */
	if (refcnt > 0)
		return;

	/* LE connections in scanning state need special handling */
	if (conn->state == BT_CONNECT && conn->type == LE_LINK &&
	    test_bit(HCI_CONN_SCANNING, &conn->flags)) {
		hci_connect_le_scan_remove(conn);
		return;
	}

	hci_abort_conn(conn, hci_proto_disconn_ind(conn));
}

/* Enter sniff mode */
static void hci_conn_idle(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     idle_work.work);
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("hcon %p mode %d", conn, conn->mode);

	if (!lmp_sniff_capable(hdev) || !lmp_sniff_capable(conn))
		return;

	if (conn->mode != HCI_CM_ACTIVE || !(conn->link_policy & HCI_LP_SNIFF))
		return;

	if (lmp_sniffsubr_capable(hdev) && lmp_sniffsubr_capable(conn)) {
		struct hci_cp_sniff_subrate cp;
		cp.handle             = cpu_to_le16(conn->handle);
		cp.max_latency        = cpu_to_le16(0);
		cp.min_remote_timeout = cpu_to_le16(0);
		cp.min_local_timeout  = cpu_to_le16(0);
		hci_send_cmd(hdev, HCI_OP_SNIFF_SUBRATE, sizeof(cp), &cp);
	}

	if (!test_and_set_bit(HCI_CONN_MODE_CHANGE_PEND, &conn->flags)) {
		struct hci_cp_sniff_mode cp;
		cp.handle       = cpu_to_le16(conn->handle);
		cp.max_interval = cpu_to_le16(hdev->sniff_max_interval);
		cp.min_interval = cpu_to_le16(hdev->sniff_min_interval);
		cp.attempt      = cpu_to_le16(4);
		cp.timeout      = cpu_to_le16(1);
		hci_send_cmd(hdev, HCI_OP_SNIFF_MODE, sizeof(cp), &cp);
	}
}

static void hci_conn_auto_accept(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     auto_accept_work.work);

	hci_send_cmd(conn->hdev, HCI_OP_USER_CONFIRM_REPLY, sizeof(conn->dst),
		     &conn->dst);
}

static void le_conn_timeout(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     le_conn_timeout.work);
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("");

	/* We could end up here due to having done directed advertising,
	 * so clean up the state if necessary. This should however only
	 * happen with broken hardware or if low duty cycle was used
	 * (which doesn't have a timeout of its own).
	 */
	if (conn->role == HCI_ROLE_SLAVE) {
		u8 enable = 0x00;
		hci_send_cmd(hdev, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable),
			     &enable);
		hci_le_conn_failed(conn, HCI_ERROR_ADVERTISING_TIMEOUT);
		return;
	}

	hci_abort_conn(conn, HCI_ERROR_REMOTE_USER_TERM);
}

struct hci_conn *hci_conn_add(struct hci_dev *hdev, int type, bdaddr_t *dst,
			      u8 role)
>>>>>>> android-4.9
{
	struct hci_conn *conn;

	BT_DBG("%s dst %pMR", hdev->name, dst);

<<<<<<< HEAD
	conn = kzalloc(sizeof(struct hci_conn), GFP_ATOMIC);
=======
	conn = kzalloc(sizeof(*conn), GFP_KERNEL);
>>>>>>> android-4.9
	if (!conn)
		return NULL;

	bacpy(&conn->dst, dst);
	bacpy(&conn->src, &hdev->bdaddr);
	conn->hdev  = hdev;
	conn->type  = type;
	conn->role  = role;
	conn->mode  = HCI_CM_ACTIVE;
	conn->state = BT_OPEN;
	conn->auth_type = HCI_AT_GENERAL_BONDING;
	conn->io_capability = hdev->io_capability;
	conn->remote_auth = 0xff;
<<<<<<< HEAD
=======
	conn->key_type = 0xff;
	conn->rssi = HCI_RSSI_INVALID;
	conn->tx_power = HCI_TX_POWER_INVALID;
	conn->max_tx_power = HCI_TX_POWER_INVALID;
>>>>>>> android-4.9

	conn->power_save = 1;
	conn->disc_timeout = HCI_DISCONN_TIMEOUT;
	conn->conn_valid = true;
	spin_lock_init(&conn->lock);
	wake_lock_init(&conn->idle_lock, WAKE_LOCK_SUSPEND, "bt_idle");

	if (conn->role == HCI_ROLE_MASTER)
		conn->out = true;

	switch (type) {
	case ACL_LINK:
		conn->pkt_type = hdev->pkt_type & ACL_PTYPE_MASK;
		conn->link_policy = hdev->link_policy;
		break;
	case LE_LINK:
		/* conn->src should reflect the local identity address */
		hci_copy_identity_address(hdev, &conn->src, &conn->src_type);
		break;
	case SCO_LINK:
		if (!pkt_type)
			pkt_type = SCO_ESCO_MASK;
	case ESCO_LINK:
		if (!pkt_type)
			pkt_type = ALL_ESCO_MASK;
		if (lmp_esco_capable(hdev)) {
			/* HCI Setup Synchronous Connection Command uses
			   reverse logic on the EDR_ESCO_MASK bits */
			conn->pkt_type = (pkt_type ^ EDR_ESCO_MASK) &
					hdev->esco_type;
		} else {
			/* Legacy HCI Add Sco Connection Command uses a
			   shifted bitmask */
			conn->pkt_type = (pkt_type << 5) & hdev->pkt_type &
					SCO_PTYPE_MASK;
		}
		break;
	}

	skb_queue_head_init(&conn->data_q);

<<<<<<< HEAD
	setup_timer(&conn->disc_timer, hci_conn_timeout, (unsigned long)conn);
	setup_timer(&conn->idle_timer, hci_conn_idle, (unsigned long)conn);
	INIT_DELAYED_WORK(&conn->rssi_update_work, hci_conn_rssi_update);
	setup_timer(&conn->encrypt_pause_timer, encryption_disabled_timeout,
			(unsigned long)conn);
=======
	INIT_LIST_HEAD(&conn->chan_list);

	INIT_DELAYED_WORK(&conn->disc_work, hci_conn_timeout);
	INIT_DELAYED_WORK(&conn->auto_accept_work, hci_conn_auto_accept);
	INIT_DELAYED_WORK(&conn->idle_work, hci_conn_idle);
	INIT_DELAYED_WORK(&conn->le_conn_timeout, le_conn_timeout);
	INIT_WORK(&conn->le_scan_cleanup, le_scan_cleanup);
>>>>>>> android-4.9

	atomic_set(&conn->refcnt, 0);

	hci_dev_hold(hdev);

	tasklet_disable(&hdev->tx_task);

	hci_conn_hash_add(hdev, conn);
	if (hdev->notify)
		hdev->notify(hdev, HCI_NOTIFY_CONN_ADD);

	hci_conn_init_sysfs(conn);

	tasklet_enable(&hdev->tx_task);

	return conn;
}

struct hci_conn *hci_le_conn_add(struct hci_dev *hdev, bdaddr_t *dst,
							__u8 addr_type)
{
	struct hci_conn *conn = hci_conn_add(hdev, LE_LINK, 0, dst);
	if (!conn)
		return NULL;

	conn->dst_type = addr_type;

	return conn;
}

int hci_conn_del(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

<<<<<<< HEAD
	BT_DBG("%s conn %p handle %d", hdev->name, conn, conn->handle);

	spin_lock_bh(&conn->lock);
	conn->conn_valid = false; /* conn data is being released */
	spin_unlock_bh(&conn->lock);

	/* Make sure no timers are running */
	del_timer(&conn->idle_timer);
	wake_lock_destroy(&conn->idle_lock);
	del_timer(&conn->disc_timer);
	del_timer(&conn->smp_timer);
	__cancel_delayed_work(&conn->rssi_update_work);
	del_timer(&conn->encrypt_pause_timer);
=======
	BT_DBG("%s hcon %p handle %d", hdev->name, conn, conn->handle);

	cancel_delayed_work_sync(&conn->disc_work);
	cancel_delayed_work_sync(&conn->auto_accept_work);
	cancel_delayed_work_sync(&conn->idle_work);
>>>>>>> android-4.9

	if (conn->type == ACL_LINK) {
		struct hci_conn *sco = conn->link;
		if (sco)
			sco->link = NULL;

		/* Unacked frames */
		hdev->acl_cnt += conn->sent;
	} else if (conn->type == LE_LINK) {
		cancel_delayed_work(&conn->le_conn_timeout);

		if (hdev->le_pkts)
			hdev->le_cnt += conn->sent;
		else
			hdev->acl_cnt += conn->sent;
	} else {
		struct hci_conn *acl = conn->link;
		if (acl) {
			acl->link = NULL;
			hci_conn_drop(acl);
		}
	}

<<<<<<< HEAD
	tasklet_disable(&hdev->tx_task);

	hci_conn_hash_del(hdev, conn);
	if (hdev->notify)
		hdev->notify(hdev, HCI_NOTIFY_CONN_DEL);
=======
	if (conn->amp_mgr)
		amp_mgr_put(conn->amp_mgr);
>>>>>>> android-4.9

	tasklet_schedule(&hdev->tx_task);

	tasklet_enable(&hdev->tx_task);

	skb_queue_purge(&conn->data_q);

<<<<<<< HEAD
	hci_conn_put_device(conn);

	hci_dev_put(hdev);

	return 0;
}

struct hci_chan *hci_chan_add(struct hci_dev *hdev)
{
	struct hci_chan *chan;

	BT_DBG("%s", hdev->name);

	chan = kzalloc(sizeof(struct hci_chan), GFP_ATOMIC);
	if (!chan)
		return NULL;

	atomic_set(&chan->refcnt, 0);

	hci_dev_hold(hdev);

	chan->hdev = hdev;

	list_add(&chan->list, &hdev->chan_list.list);

	return chan;
}
EXPORT_SYMBOL(hci_chan_add);

int hci_chan_del(struct hci_chan *chan)
{
	BT_DBG("%s chan %p", chan->hdev->name, chan);

	list_del(&chan->list);

	hci_conn_put(chan->conn);
	hci_dev_put(chan->hdev);

	kfree(chan);
=======
	/* Remove the connection from the list and cleanup its remaining
	 * state. This is a separate function since for some cases like
	 * BT_CONNECT_SCAN we *only* want the cleanup part without the
	 * rest of hci_conn_del.
	 */
	hci_conn_cleanup(conn);
>>>>>>> android-4.9

	return 0;
}

<<<<<<< HEAD
int hci_chan_put(struct hci_chan *chan)
{
	struct hci_cp_disconn_logical_link cp;
	struct hci_conn *hcon;
	u16 ll_handle;

	BT_DBG("chan %p refcnt %d", chan, atomic_read(&chan->refcnt));
	if (!atomic_dec_and_test(&chan->refcnt))
		return 0;

	hcon = chan->conn;
	ll_handle = chan->ll_handle;

	hci_chan_del(chan);

	BT_DBG("chan->conn->state %d", hcon->state);
	if (hcon->state == BT_CONNECTED) {
		cp.log_handle = cpu_to_le16(ll_handle);
		hci_send_cmd(hcon->hdev, HCI_OP_DISCONN_LOGICAL_LINK,
				sizeof(cp), &cp);
	}

	return 1;
}
EXPORT_SYMBOL(hci_chan_put);

struct hci_dev *hci_get_route(bdaddr_t *dst, bdaddr_t *src)
=======
struct hci_dev *hci_get_route(bdaddr_t *dst, bdaddr_t *src, uint8_t src_type)
>>>>>>> android-4.9
{
	int use_src = bacmp(src, BDADDR_ANY);
	struct hci_dev *hdev = NULL;
	struct list_head *p;

	BT_DBG("%pMR -> %pMR", src, dst);

	read_lock_bh(&hci_dev_list_lock);

<<<<<<< HEAD
	list_for_each(p, &hci_dev_list) {
		struct hci_dev *d = list_entry(p, struct hci_dev, list);

		if (d->dev_type != HCI_BREDR)
			continue;
		if (!test_bit(HCI_UP, &d->flags) || test_bit(HCI_RAW, &d->flags))
=======
	list_for_each_entry(d, &hci_dev_list, list) {
		if (!test_bit(HCI_UP, &d->flags) ||
		    hci_dev_test_flag(d, HCI_USER_CHANNEL) ||
		    d->dev_type != HCI_PRIMARY)
>>>>>>> android-4.9
			continue;

		/* Simple routing:
		 *   No source address - find interface with bdaddr != dst
		 *   Source address    - find interface with bdaddr == src
		 */

		if (use_src) {
			bdaddr_t id_addr;
			u8 id_addr_type;

			if (src_type == BDADDR_BREDR) {
				if (!lmp_bredr_capable(d))
					continue;
				bacpy(&id_addr, &d->bdaddr);
				id_addr_type = BDADDR_BREDR;
			} else {
				if (!lmp_le_capable(d))
					continue;

				hci_copy_identity_address(d, &id_addr,
							  &id_addr_type);

				/* Convert from HCI to three-value type */
				if (id_addr_type == ADDR_LE_DEV_PUBLIC)
					id_addr_type = BDADDR_LE_PUBLIC;
				else
					id_addr_type = BDADDR_LE_RANDOM;
			}

			if (!bacmp(&id_addr, src) && id_addr_type == src_type) {
				hdev = d; break;
			}
		} else {
			if (bacmp(&d->bdaddr, dst)) {
				hdev = d; break;
			}
		}
	}

	if (hdev)
		hdev = hci_dev_hold(hdev);

	read_unlock_bh(&hci_dev_list_lock);
	return hdev;
}
EXPORT_SYMBOL(hci_get_route);

<<<<<<< HEAD
struct hci_dev *hci_dev_get_type(u8 amp_type)
{
	struct hci_dev *hdev = NULL;
	struct hci_dev *d;

	BT_DBG("amp_type %d", amp_type);

	read_lock_bh(&hci_dev_list_lock);

	list_for_each_entry(d, &hci_dev_list, list) {
		if ((d->amp_type == amp_type) && test_bit(HCI_UP, &d->flags)) {
			hdev = d;
			break;
		}
	}

	if (hdev)
		hdev = hci_dev_hold(hdev);

	read_unlock_bh(&hci_dev_list_lock);
	return hdev;
}
EXPORT_SYMBOL(hci_dev_get_type);

struct hci_dev *hci_dev_get_amp(bdaddr_t *dst)
{
	struct hci_dev *d;
	struct hci_dev *hdev = NULL;

	BT_DBG("%s dst %s", hdev->name, batostr(dst));

	read_lock_bh(&hci_dev_list_lock);

	list_for_each_entry(d, &hci_dev_list, list) {
		struct hci_conn *conn;
		if (d->dev_type == HCI_BREDR)
			continue;
		conn = hci_conn_hash_lookup_ba(d, ACL_LINK, dst);
		if (conn) {
			hdev = d;
			break;
		}
=======
/* This function requires the caller holds hdev->lock */
void hci_le_conn_failed(struct hci_conn *conn, u8 status)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn_params *params;

	params = hci_pend_le_action_lookup(&hdev->pend_le_conns, &conn->dst,
					   conn->dst_type);
	if (params && params->conn) {
		hci_conn_drop(params->conn);
		hci_conn_put(params->conn);
		params->conn = NULL;
	}

	conn->state = BT_CLOSED;

	/* If the status indicates successful cancellation of
	 * the attempt (i.e. Unkown Connection Id) there's no point of
	 * notifying failure since we'll go back to keep trying to
	 * connect. The only exception is explicit connect requests
	 * where a timeout + cancel does indicate an actual failure.
	 */
	if (status != HCI_ERROR_UNKNOWN_CONN_ID ||
	    (params && params->explicit_connect))
		mgmt_connect_failed(hdev, &conn->dst, conn->type,
				    conn->dst_type, status);

	hci_connect_cfm(conn, status);

	hci_conn_del(conn);

	/* Since we may have temporarily stopped the background scanning in
	 * favor of connection establishment, we should restart it.
	 */
	hci_update_background_scan(hdev);

	/* Re-enable advertising in case this was a failed connection
	 * attempt as a peripheral.
	 */
	hci_req_reenable_advertising(hdev);
}

static void create_le_conn_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct hci_conn *conn;

	hci_dev_lock(hdev);

	conn = hci_lookup_le_connect(hdev);

	if (!status) {
		hci_connect_le_scan_cleanup(conn);
		goto done;
	}

	BT_ERR("HCI request failed to create LE connection: status 0x%2.2x",
	       status);

	if (!conn)
		goto done;

	hci_le_conn_failed(conn, status);

done:
	hci_dev_unlock(hdev);
}

static bool conn_use_rpa(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

	return hci_dev_test_flag(hdev, HCI_PRIVACY);
}

static void hci_req_add_le_create_conn(struct hci_request *req,
				       struct hci_conn *conn)
{
	struct hci_cp_le_create_conn cp;
	struct hci_dev *hdev = conn->hdev;
	u8 own_addr_type;

	/* Update random address, but set require_privacy to false so
	 * that we never connect with an non-resolvable address.
	 */
	if (hci_update_random_address(req, false, conn_use_rpa(conn),
				      &own_addr_type))
		return;

	memset(&cp, 0, sizeof(cp));

	/* Set window to be the same value as the interval to enable
	 * continuous scanning.
	 */
	cp.scan_interval = cpu_to_le16(hdev->le_scan_interval);
	cp.scan_window = cp.scan_interval;

	bacpy(&cp.peer_addr, &conn->dst);
	cp.peer_addr_type = conn->dst_type;
	cp.own_address_type = own_addr_type;
	cp.conn_interval_min = cpu_to_le16(conn->le_conn_min_interval);
	cp.conn_interval_max = cpu_to_le16(conn->le_conn_max_interval);
	cp.conn_latency = cpu_to_le16(conn->le_conn_latency);
	cp.supervision_timeout = cpu_to_le16(conn->le_supv_timeout);
	cp.min_ce_len = cpu_to_le16(0x0000);
	cp.max_ce_len = cpu_to_le16(0x0000);

	hci_req_add(req, HCI_OP_LE_CREATE_CONN, sizeof(cp), &cp);

	conn->state = BT_CONNECT;
	clear_bit(HCI_CONN_SCANNING, &conn->flags);
}

static void hci_req_directed_advertising(struct hci_request *req,
					 struct hci_conn *conn)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_le_set_adv_param cp;
	u8 own_addr_type;
	u8 enable;

	/* Clear the HCI_LE_ADV bit temporarily so that the
	 * hci_update_random_address knows that it's safe to go ahead
	 * and write a new random address. The flag will be set back on
	 * as soon as the SET_ADV_ENABLE HCI command completes.
	 */
	hci_dev_clear_flag(hdev, HCI_LE_ADV);

	/* Set require_privacy to false so that the remote device has a
	 * chance of identifying us.
	 */
	if (hci_update_random_address(req, false, conn_use_rpa(conn),
				      &own_addr_type) < 0)
		return;

	memset(&cp, 0, sizeof(cp));
	cp.type = LE_ADV_DIRECT_IND;
	cp.own_address_type = own_addr_type;
	cp.direct_addr_type = conn->dst_type;
	bacpy(&cp.direct_addr, &conn->dst);
	cp.channel_map = hdev->le_adv_channel_map;

	hci_req_add(req, HCI_OP_LE_SET_ADV_PARAM, sizeof(cp), &cp);

	enable = 0x01;
	hci_req_add(req, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable), &enable);

	conn->state = BT_CONNECT;
}

struct hci_conn *hci_connect_le(struct hci_dev *hdev, bdaddr_t *dst,
				u8 dst_type, u8 sec_level, u16 conn_timeout,
				u8 role)
{
	struct hci_conn_params *params;
	struct hci_conn *conn;
	struct smp_irk *irk;
	struct hci_request req;
	int err;

	/* Let's make sure that le is enabled.*/
	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		if (lmp_le_capable(hdev))
			return ERR_PTR(-ECONNREFUSED);

		return ERR_PTR(-EOPNOTSUPP);
	}

	/* Since the controller supports only one LE connection attempt at a
	 * time, we return -EBUSY if there is any connection attempt running.
	 */
	if (hci_lookup_le_connect(hdev))
		return ERR_PTR(-EBUSY);

	/* If there's already a connection object but it's not in
	 * scanning state it means it must already be established, in
	 * which case we can't do anything else except report a failure
	 * to connect.
	 */
	conn = hci_conn_hash_lookup_le(hdev, dst, dst_type);
	if (conn && !test_bit(HCI_CONN_SCANNING, &conn->flags)) {
		return ERR_PTR(-EBUSY);
	}

	/* When given an identity address with existing identity
	 * resolving key, the connection needs to be established
	 * to a resolvable random address.
	 *
	 * Storing the resolvable random address is required here
	 * to handle connection failures. The address will later
	 * be resolved back into the original identity address
	 * from the connect request.
	 */
	irk = hci_find_irk_by_addr(hdev, dst, dst_type);
	if (irk && bacmp(&irk->rpa, BDADDR_ANY)) {
		dst = &irk->rpa;
		dst_type = ADDR_LE_DEV_RANDOM;
	}

	if (conn) {
		bacpy(&conn->dst, dst);
	} else {
		conn = hci_conn_add(hdev, LE_LINK, dst, role);
		if (!conn)
			return ERR_PTR(-ENOMEM);
		hci_conn_hold(conn);
		conn->pending_sec_level = sec_level;
	}

	conn->dst_type = dst_type;
	conn->sec_level = BT_SECURITY_LOW;
	conn->conn_timeout = conn_timeout;

	hci_req_init(&req, hdev);

	/* Disable advertising if we're active. For master role
	 * connections most controllers will refuse to connect if
	 * advertising is enabled, and for slave role connections we
	 * anyway have to disable it in order to start directed
	 * advertising.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_ADV)) {
		u8 enable = 0x00;
		hci_req_add(&req, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable),
			    &enable);
	}

	/* If requested to connect as slave use directed advertising */
	if (conn->role == HCI_ROLE_SLAVE) {
		/* If we're active scanning most controllers are unable
		 * to initiate advertising. Simply reject the attempt.
		 */
		if (hci_dev_test_flag(hdev, HCI_LE_SCAN) &&
		    hdev->le_scan_type == LE_SCAN_ACTIVE) {
			skb_queue_purge(&req.cmd_q);
			hci_conn_del(conn);
			return ERR_PTR(-EBUSY);
		}

		hci_req_directed_advertising(&req, conn);
		goto create_conn;
	}

	params = hci_conn_params_lookup(hdev, &conn->dst, conn->dst_type);
	if (params) {
		conn->le_conn_min_interval = params->conn_min_interval;
		conn->le_conn_max_interval = params->conn_max_interval;
		conn->le_conn_latency = params->conn_latency;
		conn->le_supv_timeout = params->supervision_timeout;
	} else {
		conn->le_conn_min_interval = hdev->le_conn_min_interval;
		conn->le_conn_max_interval = hdev->le_conn_max_interval;
		conn->le_conn_latency = hdev->le_conn_latency;
		conn->le_supv_timeout = hdev->le_supv_timeout;
	}

	/* If controller is scanning, we stop it since some controllers are
	 * not able to scan and connect at the same time. Also set the
	 * HCI_LE_SCAN_INTERRUPTED flag so that the command complete
	 * handler for scan disabling knows to set the correct discovery
	 * state.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_SCAN)) {
		hci_req_add_le_scan_disable(&req);
		hci_dev_set_flag(hdev, HCI_LE_SCAN_INTERRUPTED);
	}

	hci_req_add_le_create_conn(&req, conn);

create_conn:
	err = hci_req_run(&req, create_le_conn_complete);
	if (err) {
		hci_conn_del(conn);
		return ERR_PTR(err);
	}

	return conn;
}

static bool is_connected(struct hci_dev *hdev, bdaddr_t *addr, u8 type)
{
	struct hci_conn *conn;

	conn = hci_conn_hash_lookup_le(hdev, addr, type);
	if (!conn)
		return false;

	if (conn->state != BT_CONNECTED)
		return false;

	return true;
}

/* This function requires the caller holds hdev->lock */
static int hci_explicit_conn_params_set(struct hci_dev *hdev,
					bdaddr_t *addr, u8 addr_type)
{
	struct hci_conn_params *params;

	if (is_connected(hdev, addr, addr_type))
		return -EISCONN;

	params = hci_conn_params_lookup(hdev, addr, addr_type);
	if (!params) {
		params = hci_conn_params_add(hdev, addr, addr_type);
		if (!params)
			return -ENOMEM;

		/* If we created new params, mark them to be deleted in
		 * hci_connect_le_scan_cleanup. It's different case than
		 * existing disabled params, those will stay after cleanup.
		 */
		params->auto_connect = HCI_AUTO_CONN_EXPLICIT;
	}

	/* We're trying to connect, so make sure params are at pend_le_conns */
	if (params->auto_connect == HCI_AUTO_CONN_DISABLED ||
	    params->auto_connect == HCI_AUTO_CONN_REPORT ||
	    params->auto_connect == HCI_AUTO_CONN_EXPLICIT) {
		list_del_init(&params->action);
		list_add(&params->action, &hdev->pend_le_conns);
	}

	params->explicit_connect = true;

	BT_DBG("addr %pMR (type %u) auto_connect %u", addr, addr_type,
	       params->auto_connect);

	return 0;
}

/* This function requires the caller holds hdev->lock */
struct hci_conn *hci_connect_le_scan(struct hci_dev *hdev, bdaddr_t *dst,
				     u8 dst_type, u8 sec_level,
				     u16 conn_timeout)
{
	struct hci_conn *conn;

	/* Let's make sure that le is enabled.*/
	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		if (lmp_le_capable(hdev))
			return ERR_PTR(-ECONNREFUSED);

		return ERR_PTR(-EOPNOTSUPP);
	}

	/* Some devices send ATT messages as soon as the physical link is
	 * established. To be able to handle these ATT messages, the user-
	 * space first establishes the connection and then starts the pairing
	 * process.
	 *
	 * So if a hci_conn object already exists for the following connection
	 * attempt, we simply update pending_sec_level and auth_type fields
	 * and return the object found.
	 */
	conn = hci_conn_hash_lookup_le(hdev, dst, dst_type);
	if (conn) {
		if (conn->pending_sec_level < sec_level)
			conn->pending_sec_level = sec_level;
		goto done;
	}

	BT_DBG("requesting refresh of dst_addr");

	conn = hci_conn_add(hdev, LE_LINK, dst, HCI_ROLE_MASTER);
	if (!conn)
		return ERR_PTR(-ENOMEM);

	if (hci_explicit_conn_params_set(hdev, dst, dst_type) < 0)
		return ERR_PTR(-EBUSY);

	conn->state = BT_CONNECT;
	set_bit(HCI_CONN_SCANNING, &conn->flags);
	conn->dst_type = dst_type;
	conn->sec_level = BT_SECURITY_LOW;
	conn->pending_sec_level = sec_level;
	conn->conn_timeout = conn_timeout;

	hci_update_background_scan(hdev);

done:
	hci_conn_hold(conn);
	return conn;
}

struct hci_conn *hci_connect_acl(struct hci_dev *hdev, bdaddr_t *dst,
				 u8 sec_level, u8 auth_type)
{
	struct hci_conn *acl;

	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		if (lmp_bredr_capable(hdev))
			return ERR_PTR(-ECONNREFUSED);

		return ERR_PTR(-EOPNOTSUPP);
>>>>>>> android-4.9
	}

	if (hdev)
		hdev = hci_dev_hold(hdev);

	read_unlock_bh(&hci_dev_list_lock);
	return hdev;
}
EXPORT_SYMBOL(hci_dev_get_amp);

/* Create SCO, ACL or LE connection.
 * Device _must_ be locked */
struct hci_conn *hci_connect(struct hci_dev *hdev, int type,
					__u16 pkt_type, bdaddr_t *dst,
					__u8 sec_level, __u8 auth_type)
{
	struct hci_conn *acl;
	struct hci_conn *sco;

	BT_DBG("%s dst %s", hdev->name, batostr(dst));

	if (type == LE_LINK)
		return hci_le_connect(hdev, pkt_type, dst, sec_level,
							auth_type, NULL);

	acl = hci_conn_hash_lookup_ba(hdev, ACL_LINK, dst);
	if (!acl) {
<<<<<<< HEAD
		acl = hci_conn_add(hdev, ACL_LINK, 0, dst);
=======
		acl = hci_conn_add(hdev, ACL_LINK, dst, HCI_ROLE_MASTER);
>>>>>>> android-4.9
		if (!acl)
			return NULL;
	}

	hci_conn_hold(acl);

	if (acl->state == BT_OPEN || acl->state == BT_CLOSED) {
		acl->sec_level = BT_SECURITY_LOW;
		acl->pending_sec_level = sec_level;
		acl->auth_type = auth_type;
		hci_acl_create_connection(acl);
	}

	return acl;
}

struct hci_conn *hci_connect_sco(struct hci_dev *hdev, int type, bdaddr_t *dst,
				 __u16 setting)
{
	struct hci_conn *acl;
	struct hci_conn *sco;

	acl = hci_connect_acl(hdev, dst, BT_SECURITY_LOW, HCI_AT_NO_BONDING);
	if (IS_ERR(acl))
		return acl;

	/* type of connection already existing can be ESCO or SCO
	 * so check for both types before creating new */

	sco = hci_conn_hash_lookup_ba(hdev, type, dst);

	if (!sco && type == ESCO_LINK) {
		sco = hci_conn_hash_lookup_ba(hdev, SCO_LINK, dst);
	} else if (!sco && type == SCO_LINK) {
		/* this case can be practically not possible */
		sco = hci_conn_hash_lookup_ba(hdev, ESCO_LINK, dst);
	}

	if (!sco) {
<<<<<<< HEAD
		sco = hci_conn_add(hdev, type, pkt_type, dst);
		if (!sco) {
			hci_conn_put(acl);
			return NULL;
=======
		sco = hci_conn_add(hdev, type, dst, HCI_ROLE_MASTER);
		if (!sco) {
			hci_conn_drop(acl);
			return ERR_PTR(-ENOMEM);
>>>>>>> android-4.9
		}
	}

	acl->link = sco;
	sco->link = acl;

	hci_conn_hold(sco);

	sco->setting = setting;

	if (acl->state == BT_CONNECTED &&
<<<<<<< HEAD
			(sco->state == BT_OPEN || sco->state == BT_CLOSED)) {
		acl->power_save = 1;
		hci_conn_enter_active_mode(acl, 1);
=======
	    (sco->state == BT_OPEN || sco->state == BT_CLOSED)) {
		set_bit(HCI_CONN_POWER_SAVE, &acl->flags);
		hci_conn_enter_active_mode(acl, BT_POWER_FORCE_ACTIVE_ON);
>>>>>>> android-4.9

		if (test_bit(HCI_CONN_MODE_CHANGE_PEND, &acl->pend)) {
			/* defer SCO setup until mode change completed */
			set_bit(HCI_CONN_SCO_SETUP_PEND, &acl->pend);
			return sco;
		}

		hci_sco_setup(acl, 0x00);
	}

	return sco;
}

void hci_disconnect(struct hci_conn *conn, __u8 reason)
{
	BT_DBG("conn %p", conn);

	hci_proto_disconn_cfm(conn, reason, 0);
}
EXPORT_SYMBOL(hci_disconnect);

void hci_disconnect_amp(struct hci_conn *conn, __u8 reason)
{
	struct hci_dev *hdev = NULL;

	BT_DBG("conn %p", conn);

	read_lock_bh(&hci_dev_list_lock);

	list_for_each_entry(hdev, &hci_dev_list, list) {
		struct hci_conn *c;
		if (hdev == conn->hdev)
			continue;
		if (hdev->amp_type == HCI_BREDR)
			continue;
		c = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &conn->dst);
		if (c)
			hci_disconnect(c, reason);
	}

	read_unlock_bh(&hci_dev_list_lock);
}

/* Check link security requirement */
int hci_conn_check_link_mode(struct hci_conn *conn)
{
	BT_DBG("hcon %p", conn);

<<<<<<< HEAD
	if (conn->ssp_mode > 0 && conn->hdev->ssp_mode > 0 &&
					!(conn->link_mode & HCI_LM_ENCRYPT))
=======
	/* In Secure Connections Only mode, it is required that Secure
	 * Connections is used and the link is encrypted with AES-CCM
	 * using a P-256 authenticated combination key.
	 */
	if (hci_dev_test_flag(conn->hdev, HCI_SC_ONLY)) {
		if (!hci_conn_sc_enabled(conn) ||
		    !test_bit(HCI_CONN_AES_CCM, &conn->flags) ||
		    conn->key_type != HCI_LK_AUTH_COMBINATION_P256)
			return 0;
	}

	if (hci_conn_ssp_enabled(conn) &&
	    !test_bit(HCI_CONN_ENCRYPT, &conn->flags))
>>>>>>> android-4.9
		return 0;

	return 1;
}

/* Authenticate remote device */
static int hci_conn_auth(struct hci_conn *conn, __u8 sec_level, __u8 auth_type)
{
	BT_DBG("hcon %p", conn);

	if (conn->pending_sec_level > sec_level)
		sec_level = conn->pending_sec_level;

	if (sec_level > conn->sec_level)
		conn->pending_sec_level = sec_level;
	else if (test_bit(HCI_CONN_AUTH, &conn->flags))
		return 1;

	/* Make sure we preserve an existing MITM requirement*/
	auth_type |= (conn->auth_type & 0x01);
	conn->auth_type = auth_type;
	conn->auth_initiator = 1;

	if (!test_and_set_bit(HCI_CONN_AUTH_PEND, &conn->pend)) {
		struct hci_cp_auth_requested cp;

<<<<<<< HEAD
		/* encrypt must be pending if auth is also pending */
		set_bit(HCI_CONN_ENCRYPT_PEND, &conn->pend);

		cp.handle = cpu_to_le16(conn->handle);
		hci_send_cmd(conn->hdev, HCI_OP_AUTH_REQUESTED,
							sizeof(cp), &cp);
=======
		cp.handle = cpu_to_le16(conn->handle);
		hci_send_cmd(conn->hdev, HCI_OP_AUTH_REQUESTED,
			     sizeof(cp), &cp);

		/* If we're already encrypted set the REAUTH_PEND flag,
		 * otherwise set the ENCRYPT_PEND.
		 */
		if (test_bit(HCI_CONN_ENCRYPT, &conn->flags))
			set_bit(HCI_CONN_REAUTH_PEND, &conn->flags);
		else
			set_bit(HCI_CONN_ENCRYPT_PEND, &conn->flags);
>>>>>>> android-4.9
	}

	return 0;
}

<<<<<<< HEAD
=======
/* Encrypt the the link */
static void hci_conn_encrypt(struct hci_conn *conn)
{
	BT_DBG("hcon %p", conn);

	if (!test_and_set_bit(HCI_CONN_ENCRYPT_PEND, &conn->flags)) {
		struct hci_cp_set_conn_encrypt cp;
		cp.handle  = cpu_to_le16(conn->handle);
		cp.encrypt = 0x01;
		hci_send_cmd(conn->hdev, HCI_OP_SET_CONN_ENCRYPT, sizeof(cp),
			     &cp);
	}
}

>>>>>>> android-4.9
/* Enable security */
int hci_conn_security(struct hci_conn *conn, __u8 sec_level, __u8 auth_type,
		      bool initiator)
{
<<<<<<< HEAD
	BT_DBG("conn %p %d %d", conn, sec_level, auth_type);
=======
	BT_DBG("hcon %p", conn);

	if (conn->type == LE_LINK)
		return smp_conn_security(conn, sec_level);
>>>>>>> android-4.9

	if (sec_level == BT_SECURITY_SDP)
		return 1;

	if (sec_level == BT_SECURITY_LOW &&
				(!conn->ssp_mode || !conn->hdev->ssp_mode))
		return 1;

<<<<<<< HEAD
	if (conn->type == LE_LINK) {
		if (conn->pending_sec_level > sec_level)
			sec_level = conn->pending_sec_level;

		if (sec_level > conn->sec_level)
			conn->pending_sec_level = sec_level;
		hci_proto_connect_cfm(conn, 0);
		return 0;
	} else if (conn->link_mode & HCI_LM_ENCRYPT) {
		return hci_conn_auth(conn, sec_level, auth_type);
	} else if (test_bit(HCI_CONN_ENCRYPT_PEND, &conn->pend)) {
=======
	/* For other security levels we need the link key. */
	if (!test_bit(HCI_CONN_AUTH, &conn->flags))
		goto auth;

	/* An authenticated FIPS approved combination key has sufficient
	 * security for security level 4. */
	if (conn->key_type == HCI_LK_AUTH_COMBINATION_P256 &&
	    sec_level == BT_SECURITY_FIPS)
		goto encrypt;

	/* An authenticated combination key has sufficient security for
	   security level 3. */
	if ((conn->key_type == HCI_LK_AUTH_COMBINATION_P192 ||
	     conn->key_type == HCI_LK_AUTH_COMBINATION_P256) &&
	    sec_level == BT_SECURITY_HIGH)
		goto encrypt;

	/* An unauthenticated combination key has sufficient security for
	   security level 1 and 2. */
	if ((conn->key_type == HCI_LK_UNAUTH_COMBINATION_P192 ||
	     conn->key_type == HCI_LK_UNAUTH_COMBINATION_P256) &&
	    (sec_level == BT_SECURITY_MEDIUM || sec_level == BT_SECURITY_LOW))
		goto encrypt;

	/* A combination key has always sufficient security for the security
	   levels 1 or 2. High security level requires the combination key
	   is generated using maximum PIN code length (16).
	   For pre 2.1 units. */
	if (conn->key_type == HCI_LK_COMBINATION &&
	    (sec_level == BT_SECURITY_MEDIUM || sec_level == BT_SECURITY_LOW ||
	     conn->pin_length == 16))
		goto encrypt;

auth:
	if (test_bit(HCI_CONN_ENCRYPT_PEND, &conn->flags))
		return 0;

	if (initiator)
		set_bit(HCI_CONN_AUTH_INITIATOR, &conn->flags);

	if (!hci_conn_auth(conn, sec_level, auth_type))
>>>>>>> android-4.9
		return 0;
	}

<<<<<<< HEAD
	if (hci_conn_auth(conn, sec_level, auth_type)) {
		struct hci_cp_set_conn_encrypt cp;
		if (timer_pending(&conn->encrypt_pause_timer)) {
			BT_INFO("encrypt_pause_timer is pending");
			return 0;
		}
		cp.handle  = cpu_to_le16(conn->handle);
		cp.encrypt = 1;
		hci_send_cmd(conn->hdev, HCI_OP_SET_CONN_ENCRYPT,
							sizeof(cp), &cp);
	}
=======
encrypt:
	if (test_bit(HCI_CONN_ENCRYPT, &conn->flags))
		return 1;
>>>>>>> android-4.9

	return 0;
}
EXPORT_SYMBOL(hci_conn_security);

<<<<<<< HEAD
/* Change link key */
int hci_conn_change_link_key(struct hci_conn *conn)
{
	BT_DBG("conn %p", conn);

	if (!test_and_set_bit(HCI_CONN_AUTH_PEND, &conn->pend)) {
		struct hci_cp_change_conn_link_key cp;
		cp.handle = cpu_to_le16(conn->handle);
		hci_send_cmd(conn->hdev, HCI_OP_CHANGE_CONN_LINK_KEY,
							sizeof(cp), &cp);
	}
=======
/* Check secure link requirement */
int hci_conn_check_secure(struct hci_conn *conn, __u8 sec_level)
{
	BT_DBG("hcon %p", conn);

	/* Accept if non-secure or higher security level is required */
	if (sec_level != BT_SECURITY_HIGH && sec_level != BT_SECURITY_FIPS)
		return 1;

	/* Accept if secure or higher security level is already present */
	if (conn->sec_level == BT_SECURITY_HIGH ||
	    conn->sec_level == BT_SECURITY_FIPS)
		return 1;
>>>>>>> android-4.9

	/* Reject not secure link */
	return 0;
}
EXPORT_SYMBOL(hci_conn_check_secure);

/* Switch role */
int hci_conn_switch_role(struct hci_conn *conn, __u8 role)
{
	BT_DBG("hcon %p", conn);

	if (role == conn->role)
		return 1;

	if (!test_and_set_bit(HCI_CONN_RSWITCH_PEND, &conn->pend)) {
		struct hci_cp_switch_role cp;
		bacpy(&cp.bdaddr, &conn->dst);
		cp.role = role;
		hci_send_cmd(conn->hdev, HCI_OP_SWITCH_ROLE, sizeof(cp), &cp);
	}

	return 0;
}
EXPORT_SYMBOL(hci_conn_switch_role);

/* Enter active mode */
void hci_conn_enter_active_mode(struct hci_conn *conn, __u8 force_active)
{
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("hcon %p mode %d", conn, conn->mode);

	if (conn->type == LE_LINK)
		return;

	if (conn->mode != HCI_CM_SNIFF)
		goto timer;

	if (!conn->power_save && !force_active)
		goto timer;

	if (!test_and_set_bit(HCI_CONN_MODE_CHANGE_PEND, &conn->pend)) {
		struct hci_cp_exit_sniff_mode cp;
		cp.handle = cpu_to_le16(conn->handle);
		hci_send_cmd(hdev, HCI_OP_EXIT_SNIFF_MODE, sizeof(cp), &cp);
	}

timer:
<<<<<<< HEAD
	if (hdev->idle_timeout > 0) {
		spin_lock_bh(&conn->lock);
		if (conn->conn_valid) {
			mod_timer(&conn->idle_timer,
				jiffies + msecs_to_jiffies(hdev->idle_timeout));
			wake_lock(&conn->idle_lock);
		}
		spin_unlock_bh(&conn->lock);
	}
=======
	if (hdev->idle_timeout > 0)
		queue_delayed_work(hdev->workqueue, &conn->idle_work,
				   msecs_to_jiffies(hdev->idle_timeout));
>>>>>>> android-4.9
}

static inline void hci_conn_stop_rssi_timer(struct hci_conn *conn)
{
	BT_DBG("conn %p", conn);
	cancel_delayed_work(&conn->rssi_update_work);
}

static inline void hci_conn_start_rssi_timer(struct hci_conn *conn,
	u16 interval)
{
	struct hci_dev *hdev = conn->hdev;
	BT_DBG("conn %p, pending %d", conn,
			delayed_work_pending(&conn->rssi_update_work));
	if (!delayed_work_pending(&conn->rssi_update_work)) {
		queue_delayed_work(hdev->workqueue, &conn->rssi_update_work,
				msecs_to_jiffies(interval));
	}
}

void hci_conn_set_rssi_reporter(struct hci_conn *conn,
	s8 rssi_threshold, u16 interval, u8 updateOnThreshExceed)
{
	if (conn) {
		conn->rssi_threshold = rssi_threshold;
		conn->rssi_update_interval = interval;
		conn->rssi_update_thresh_exceed = updateOnThreshExceed;
		hci_conn_start_rssi_timer(conn, interval);
	}
}

void hci_conn_unset_rssi_reporter(struct hci_conn *conn)
{
	if (conn) {
		BT_DBG("Deleting the rssi_update_timer");
		hci_conn_stop_rssi_timer(conn);
	}
}

/* Enter sniff mode */
void hci_conn_enter_sniff_mode(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("conn %p mode %d", conn, conn->mode);

	if (test_bit(HCI_RAW, &hdev->flags))
		return;

	if (conn->type == LE_LINK)
		return;

	if (!lmp_sniff_capable(hdev) || !lmp_sniff_capable(conn))
		return;

	if (conn->mode != HCI_CM_ACTIVE ||
		!(conn->link_policy & HCI_LP_SNIFF) ||
		(hci_find_link_key(hdev, &conn->dst) == NULL))
		return;

	if (lmp_sniffsubr_capable(hdev) && lmp_sniffsubr_capable(conn)) {
		struct hci_cp_sniff_subrate cp;
		cp.handle             = cpu_to_le16(conn->handle);
		cp.max_latency        = cpu_to_le16(0);
		cp.min_remote_timeout = cpu_to_le16(0);
		cp.min_local_timeout  = cpu_to_le16(0);
		hci_send_cmd(hdev, HCI_OP_SNIFF_SUBRATE, sizeof(cp), &cp);
	}

	if (!test_and_set_bit(HCI_CONN_MODE_CHANGE_PEND, &conn->pend)) {
		struct hci_cp_sniff_mode cp;
		cp.handle       = cpu_to_le16(conn->handle);
		cp.max_interval = cpu_to_le16(hdev->sniff_max_interval);
		cp.min_interval = cpu_to_le16(hdev->sniff_min_interval);
		cp.attempt      = cpu_to_le16(4);
		cp.timeout      = cpu_to_le16(1);
		hci_send_cmd(hdev, HCI_OP_SNIFF_MODE, sizeof(cp), &cp);
	}
}

struct hci_chan *hci_chan_create(struct hci_chan *chan,
			struct hci_ext_fs *tx_fs, struct hci_ext_fs *rx_fs)
{
	struct hci_cp_create_logical_link cp;

	chan->state = BT_CONNECT;
	chan->tx_fs = *tx_fs;
	chan->rx_fs = *rx_fs;
	cp.phy_handle = chan->conn->handle;
	cp.tx_fs.id = chan->tx_fs.id;
	cp.tx_fs.type = chan->tx_fs.type;
	cp.tx_fs.max_sdu = cpu_to_le16(chan->tx_fs.max_sdu);
	cp.tx_fs.sdu_arr_time = cpu_to_le32(chan->tx_fs.sdu_arr_time);
	cp.tx_fs.acc_latency = cpu_to_le32(chan->tx_fs.acc_latency);
	cp.tx_fs.flush_to = cpu_to_le32(chan->tx_fs.flush_to);
	cp.rx_fs.id = chan->rx_fs.id;
	cp.rx_fs.type = chan->rx_fs.type;
	cp.rx_fs.max_sdu = cpu_to_le16(chan->rx_fs.max_sdu);
	cp.rx_fs.sdu_arr_time = cpu_to_le32(chan->rx_fs.sdu_arr_time);
	cp.rx_fs.acc_latency = cpu_to_le32(chan->rx_fs.acc_latency);
	cp.rx_fs.flush_to = cpu_to_le32(chan->rx_fs.flush_to);
	hci_conn_hold(chan->conn);
	if (chan->conn->out)
		hci_send_cmd(chan->conn->hdev, HCI_OP_CREATE_LOGICAL_LINK,
							sizeof(cp), &cp);
	else
		hci_send_cmd(chan->conn->hdev, HCI_OP_ACCEPT_LOGICAL_LINK,
							sizeof(cp), &cp);
	return chan;
}
EXPORT_SYMBOL(hci_chan_create);

void hci_chan_modify(struct hci_chan *chan,
			struct hci_ext_fs *tx_fs, struct hci_ext_fs *rx_fs)
{
	struct hci_cp_flow_spec_modify cp;

	chan->tx_fs = *tx_fs;
	chan->rx_fs = *rx_fs;
	cp.log_handle = cpu_to_le16(chan->ll_handle);
	cp.tx_fs.id = tx_fs->id;
	cp.tx_fs.type = tx_fs->type;
	cp.tx_fs.max_sdu = cpu_to_le16(tx_fs->max_sdu);
	cp.tx_fs.sdu_arr_time = cpu_to_le32(tx_fs->sdu_arr_time);
	cp.tx_fs.acc_latency = cpu_to_le32(tx_fs->acc_latency);
	cp.tx_fs.flush_to = cpu_to_le32(tx_fs->flush_to);
	cp.rx_fs.id = rx_fs->id;
	cp.rx_fs.type = rx_fs->type;
	cp.rx_fs.max_sdu = cpu_to_le16(rx_fs->max_sdu);
	cp.rx_fs.sdu_arr_time = cpu_to_le32(rx_fs->sdu_arr_time);
	cp.rx_fs.acc_latency = cpu_to_le32(rx_fs->acc_latency);
	cp.rx_fs.flush_to = cpu_to_le32(rx_fs->flush_to);
	hci_conn_hold(chan->conn);
	hci_send_cmd(chan->conn->hdev, HCI_OP_FLOW_SPEC_MODIFY, sizeof(cp),
									&cp);
}
EXPORT_SYMBOL(hci_chan_modify);

/* Drop all connection on the device */
void hci_conn_hash_flush(struct hci_dev *hdev, u8 is_process)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct list_head *p;

	BT_DBG("hdev %s", hdev->name);

	p = h->list.next;
	while (p != &h->list) {
		struct hci_conn *c;

		c = list_entry(p, struct hci_conn, list);
		p = p->next;

		c->state = BT_CLOSED;

<<<<<<< HEAD
		hci_proto_disconn_cfm(c, 0x16, is_process);
=======
		hci_disconn_cfm(c, HCI_ERROR_LOCAL_HOST_TERM);
>>>>>>> android-4.9
		hci_conn_del(c);
	}
}

/* Check pending connect attempts */
void hci_conn_check_pending(struct hci_dev *hdev)
{
	struct hci_conn *conn;

	BT_DBG("hdev %s", hdev->name);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_state(hdev, ACL_LINK, BT_CONNECT2);
	if (conn)
		hci_acl_create_connection(conn);

	hci_dev_unlock(hdev);
}

static u32 get_link_mode(struct hci_conn *conn)
{
	u32 link_mode = 0;

	if (conn->role == HCI_ROLE_MASTER)
		link_mode |= HCI_LM_MASTER;

	if (test_bit(HCI_CONN_ENCRYPT, &conn->flags))
		link_mode |= HCI_LM_ENCRYPT;

	if (test_bit(HCI_CONN_AUTH, &conn->flags))
		link_mode |= HCI_LM_AUTH;

	if (test_bit(HCI_CONN_SECURE, &conn->flags))
		link_mode |= HCI_LM_SECURE;

	if (test_bit(HCI_CONN_FIPS, &conn->flags))
		link_mode |= HCI_LM_FIPS;

	return link_mode;
}

int hci_get_conn_list(void __user *arg)
{
<<<<<<< HEAD
=======
	struct hci_conn *c;
>>>>>>> android-4.9
	struct hci_conn_list_req req, *cl;
	struct hci_conn_info *ci;
	struct hci_dev *hdev;
	struct list_head *p;
	int n = 0, size, err;

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;

	if (!req.conn_num || req.conn_num > (PAGE_SIZE * 2) / sizeof(*ci))
		return -EINVAL;

	size = sizeof(req) + req.conn_num * sizeof(*ci);

	cl = kmalloc(size, GFP_KERNEL);
	if (!cl)
		return -ENOMEM;

	hdev = hci_dev_get(req.dev_id);
	if (!hdev) {
		kfree(cl);
		return -ENODEV;
	}

	ci = cl->conn_info;

	hci_dev_lock_bh(hdev);
	list_for_each(p, &hdev->conn_hash.list) {
		register struct hci_conn *c;
		c = list_entry(p, struct hci_conn, list);

		bacpy(&(ci + n)->bdaddr, &c->dst);
		(ci + n)->handle = c->handle;
		(ci + n)->type  = c->type;
		(ci + n)->out   = c->out;
		(ci + n)->state = c->state;
<<<<<<< HEAD
		(ci + n)->link_mode = c->link_mode;
		if (c->type == SCO_LINK) {
			(ci + n)->mtu = hdev->sco_mtu;
			(ci + n)->cnt = hdev->sco_cnt;
			(ci + n)->pkts = hdev->sco_pkts;
		} else {
			(ci + n)->mtu = hdev->acl_mtu;
			(ci + n)->cnt = hdev->acl_cnt;
			(ci + n)->pkts = hdev->acl_pkts;
		}
=======
		(ci + n)->link_mode = get_link_mode(c);
>>>>>>> android-4.9
		if (++n >= req.conn_num)
			break;
	}
	hci_dev_unlock_bh(hdev);

	cl->dev_id = hdev->id;
	cl->conn_num = n;
	size = sizeof(req) + n * sizeof(*ci);

	hci_dev_put(hdev);

	err = copy_to_user(arg, cl, size);
	kfree(cl);

	return err ? -EFAULT : 0;
}

int hci_get_conn_info(struct hci_dev *hdev, void __user *arg)
{
	struct hci_conn_info_req req;
	struct hci_conn_info ci;
	struct hci_conn *conn;
	char __user *ptr = arg + sizeof(req);

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;

	hci_dev_lock_bh(hdev);
	conn = hci_conn_hash_lookup_ba(hdev, req.type, &req.bdaddr);
	if (conn) {
		bacpy(&ci.bdaddr, &conn->dst);
		ci.handle = conn->handle;
		ci.type  = conn->type;
		ci.out   = conn->out;
		ci.state = conn->state;
<<<<<<< HEAD
		ci.link_mode = conn->link_mode;
		if (req.type == SCO_LINK) {
			ci.mtu = hdev->sco_mtu;
			ci.cnt = hdev->sco_cnt;
			ci.pkts = hdev->sco_pkts;
		} else {
			ci.mtu = hdev->acl_mtu;
			ci.cnt = hdev->acl_cnt;
			ci.pkts = hdev->acl_pkts;
		}
		ci.pending_sec_level = conn->pending_sec_level;
		ci.ssp_mode = conn->ssp_mode;
=======
		ci.link_mode = get_link_mode(conn);
>>>>>>> android-4.9
	}
	hci_dev_unlock_bh(hdev);

	if (!conn)
		return -ENOENT;

	return copy_to_user(ptr, &ci, sizeof(ci)) ? -EFAULT : 0;
}

int hci_get_auth_info(struct hci_dev *hdev, void __user *arg)
{
	struct hci_auth_info_req req;
	struct hci_conn *conn;

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;

	hci_dev_lock_bh(hdev);
	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &req.bdaddr);
	if (conn)
		req.type = conn->auth_type;
	hci_dev_unlock_bh(hdev);

	if (!conn)
		return -ENOENT;

	return copy_to_user(arg, &req, sizeof(req)) ? -EFAULT : 0;
}

<<<<<<< HEAD
int hci_set_auth_info(struct hci_dev *hdev, void __user *arg)
{
	struct hci_auth_info_req req;
	struct hci_conn *conn;

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;
=======
struct hci_chan *hci_chan_create(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_chan *chan;

	BT_DBG("%s hcon %p", hdev->name, conn);

	if (test_bit(HCI_CONN_DROP, &conn->flags)) {
		BT_DBG("Refusing to create new hci_chan");
		return NULL;
	}

	chan = kzalloc(sizeof(*chan), GFP_KERNEL);
	if (!chan)
		return NULL;

	chan->conn = hci_conn_get(conn);
	skb_queue_head_init(&chan->data_q);
	chan->state = BT_CONNECTED;

	list_add_rcu(&chan->list, &conn->chan_list);

	return chan;
}

void hci_chan_del(struct hci_chan *chan)
{
	struct hci_conn *conn = chan->conn;
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("%s hcon %p chan %p", hdev->name, conn, chan);

	list_del_rcu(&chan->list);

	synchronize_rcu();

	/* Prevent new hci_chan's to be created for this hci_conn */
	set_bit(HCI_CONN_DROP, &conn->flags);

	hci_conn_put(conn);

	skb_queue_purge(&chan->data_q);
	kfree(chan);
}
>>>>>>> android-4.9

	hci_dev_lock_bh(hdev);
	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &req.bdaddr);
	if (conn) {
		conn->auth_type = req.type;
		switch (conn->auth_type) {
		case HCI_AT_NO_BONDING:
			conn->pending_sec_level = BT_SECURITY_LOW;
			break;
		case HCI_AT_DEDICATED_BONDING:
		case HCI_AT_GENERAL_BONDING:
			conn->pending_sec_level = BT_SECURITY_MEDIUM;
			break;
		case HCI_AT_DEDICATED_BONDING_MITM:
		case HCI_AT_GENERAL_BONDING_MITM:
			conn->pending_sec_level = BT_SECURITY_HIGH;
			break;
		default:
			break;
		}
	}
	hci_dev_unlock_bh(hdev);

<<<<<<< HEAD
	if (!conn)
		return -ENOENT;
=======
	BT_DBG("hcon %p", conn);
>>>>>>> android-4.9

	return copy_to_user(arg, &req, sizeof(req)) ? -EFAULT : 0;
}

static struct hci_chan *__hci_chan_lookup_handle(struct hci_conn *hcon,
						 __u16 handle)
{
	struct hci_chan *hchan;

	list_for_each_entry(hchan, &hcon->chan_list, list) {
		if (hchan->handle == handle)
			return hchan;
	}

	return NULL;
}

struct hci_chan *hci_chan_lookup_handle(struct hci_dev *hdev, __u16 handle)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn *hcon;
	struct hci_chan *hchan = NULL;

	rcu_read_lock();

	list_for_each_entry_rcu(hcon, &h->list, list) {
		hchan = __hci_chan_lookup_handle(hcon, handle);
		if (hchan)
			break;
	}

	rcu_read_unlock();

	return hchan;
}
