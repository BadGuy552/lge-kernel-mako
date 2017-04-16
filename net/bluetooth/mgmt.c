/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (C) 2010  Nokia Corporation
   Copyright (c) 2011-2012 The Linux Foundation.  All rights reserved.

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

/* Bluetooth HCI Management interface */

<<<<<<< HEAD
#include <linux/uaccess.h>
#include <linux/interrupt.h>
=======
>>>>>>> android-4.9
#include <linux/module.h>
#include <asm/unaligned.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
<<<<<<< HEAD
=======
#include <net/bluetooth/hci_sock.h>
>>>>>>> android-4.9
#include <net/bluetooth/l2cap.h>
#include <net/bluetooth/mgmt.h>

<<<<<<< HEAD
#define MGMT_VERSION	0
#define MGMT_REVISION	1

#define SCAN_IDLE	0x00
#define SCAN_LE		0x01
#define SCAN_BR		0x02

struct pending_cmd {
	struct list_head list;
	__u16 opcode;
	int index;
	void *param;
	struct sock *sk;
	void *user_data;
};

struct mgmt_pending_free_work {
	struct work_struct work;
	struct sock *sk;
=======
#include "hci_request.h"
#include "smp.h"
#include "mgmt_util.h"

#define MGMT_VERSION	1
#define MGMT_REVISION	14

static const u16 mgmt_commands[] = {
	MGMT_OP_READ_INDEX_LIST,
	MGMT_OP_READ_INFO,
	MGMT_OP_SET_POWERED,
	MGMT_OP_SET_DISCOVERABLE,
	MGMT_OP_SET_CONNECTABLE,
	MGMT_OP_SET_FAST_CONNECTABLE,
	MGMT_OP_SET_BONDABLE,
	MGMT_OP_SET_LINK_SECURITY,
	MGMT_OP_SET_SSP,
	MGMT_OP_SET_HS,
	MGMT_OP_SET_LE,
	MGMT_OP_SET_DEV_CLASS,
	MGMT_OP_SET_LOCAL_NAME,
	MGMT_OP_ADD_UUID,
	MGMT_OP_REMOVE_UUID,
	MGMT_OP_LOAD_LINK_KEYS,
	MGMT_OP_LOAD_LONG_TERM_KEYS,
	MGMT_OP_DISCONNECT,
	MGMT_OP_GET_CONNECTIONS,
	MGMT_OP_PIN_CODE_REPLY,
	MGMT_OP_PIN_CODE_NEG_REPLY,
	MGMT_OP_SET_IO_CAPABILITY,
	MGMT_OP_PAIR_DEVICE,
	MGMT_OP_CANCEL_PAIR_DEVICE,
	MGMT_OP_UNPAIR_DEVICE,
	MGMT_OP_USER_CONFIRM_REPLY,
	MGMT_OP_USER_CONFIRM_NEG_REPLY,
	MGMT_OP_USER_PASSKEY_REPLY,
	MGMT_OP_USER_PASSKEY_NEG_REPLY,
	MGMT_OP_READ_LOCAL_OOB_DATA,
	MGMT_OP_ADD_REMOTE_OOB_DATA,
	MGMT_OP_REMOVE_REMOTE_OOB_DATA,
	MGMT_OP_START_DISCOVERY,
	MGMT_OP_STOP_DISCOVERY,
	MGMT_OP_CONFIRM_NAME,
	MGMT_OP_BLOCK_DEVICE,
	MGMT_OP_UNBLOCK_DEVICE,
	MGMT_OP_SET_DEVICE_ID,
	MGMT_OP_SET_ADVERTISING,
	MGMT_OP_SET_BREDR,
	MGMT_OP_SET_STATIC_ADDRESS,
	MGMT_OP_SET_SCAN_PARAMS,
	MGMT_OP_SET_SECURE_CONN,
	MGMT_OP_SET_DEBUG_KEYS,
	MGMT_OP_SET_PRIVACY,
	MGMT_OP_LOAD_IRKS,
	MGMT_OP_GET_CONN_INFO,
	MGMT_OP_GET_CLOCK_INFO,
	MGMT_OP_ADD_DEVICE,
	MGMT_OP_REMOVE_DEVICE,
	MGMT_OP_LOAD_CONN_PARAM,
	MGMT_OP_READ_UNCONF_INDEX_LIST,
	MGMT_OP_READ_CONFIG_INFO,
	MGMT_OP_SET_EXTERNAL_CONFIG,
	MGMT_OP_SET_PUBLIC_ADDRESS,
	MGMT_OP_START_SERVICE_DISCOVERY,
	MGMT_OP_READ_LOCAL_OOB_EXT_DATA,
	MGMT_OP_READ_EXT_INDEX_LIST,
	MGMT_OP_READ_ADV_FEATURES,
	MGMT_OP_ADD_ADVERTISING,
	MGMT_OP_REMOVE_ADVERTISING,
	MGMT_OP_GET_ADV_SIZE_INFO,
	MGMT_OP_START_LIMITED_DISCOVERY,
	MGMT_OP_READ_EXT_INFO,
	MGMT_OP_SET_APPEARANCE,
};

static const u16 mgmt_events[] = {
	MGMT_EV_CONTROLLER_ERROR,
	MGMT_EV_INDEX_ADDED,
	MGMT_EV_INDEX_REMOVED,
	MGMT_EV_NEW_SETTINGS,
	MGMT_EV_CLASS_OF_DEV_CHANGED,
	MGMT_EV_LOCAL_NAME_CHANGED,
	MGMT_EV_NEW_LINK_KEY,
	MGMT_EV_NEW_LONG_TERM_KEY,
	MGMT_EV_DEVICE_CONNECTED,
	MGMT_EV_DEVICE_DISCONNECTED,
	MGMT_EV_CONNECT_FAILED,
	MGMT_EV_PIN_CODE_REQUEST,
	MGMT_EV_USER_CONFIRM_REQUEST,
	MGMT_EV_USER_PASSKEY_REQUEST,
	MGMT_EV_AUTH_FAILED,
	MGMT_EV_DEVICE_FOUND,
	MGMT_EV_DISCOVERING,
	MGMT_EV_DEVICE_BLOCKED,
	MGMT_EV_DEVICE_UNBLOCKED,
	MGMT_EV_DEVICE_UNPAIRED,
	MGMT_EV_PASSKEY_NOTIFY,
	MGMT_EV_NEW_IRK,
	MGMT_EV_NEW_CSRK,
	MGMT_EV_DEVICE_ADDED,
	MGMT_EV_DEVICE_REMOVED,
	MGMT_EV_NEW_CONN_PARAM,
	MGMT_EV_UNCONF_INDEX_ADDED,
	MGMT_EV_UNCONF_INDEX_REMOVED,
	MGMT_EV_NEW_CONFIG_OPTIONS,
	MGMT_EV_EXT_INDEX_ADDED,
	MGMT_EV_EXT_INDEX_REMOVED,
	MGMT_EV_LOCAL_OOB_DATA_UPDATED,
	MGMT_EV_ADVERTISING_ADDED,
	MGMT_EV_ADVERTISING_REMOVED,
	MGMT_EV_EXT_INFO_CHANGED,
};

static const u16 mgmt_untrusted_commands[] = {
	MGMT_OP_READ_INDEX_LIST,
	MGMT_OP_READ_INFO,
	MGMT_OP_READ_UNCONF_INDEX_LIST,
	MGMT_OP_READ_CONFIG_INFO,
	MGMT_OP_READ_EXT_INDEX_LIST,
	MGMT_OP_READ_EXT_INFO,
};

static const u16 mgmt_untrusted_events[] = {
	MGMT_EV_INDEX_ADDED,
	MGMT_EV_INDEX_REMOVED,
	MGMT_EV_NEW_SETTINGS,
	MGMT_EV_CLASS_OF_DEV_CHANGED,
	MGMT_EV_LOCAL_NAME_CHANGED,
	MGMT_EV_UNCONF_INDEX_ADDED,
	MGMT_EV_UNCONF_INDEX_REMOVED,
	MGMT_EV_NEW_CONFIG_OPTIONS,
	MGMT_EV_EXT_INDEX_ADDED,
	MGMT_EV_EXT_INDEX_REMOVED,
	MGMT_EV_EXT_INFO_CHANGED,
};

#define CACHE_TIMEOUT	msecs_to_jiffies(2 * 1000)

#define ZERO_KEY "\x00\x00\x00\x00\x00\x00\x00\x00" \
		 "\x00\x00\x00\x00\x00\x00\x00\x00"

/* HCI to MGMT error code conversion table */
static u8 mgmt_status_table[] = {
	MGMT_STATUS_SUCCESS,
	MGMT_STATUS_UNKNOWN_COMMAND,	/* Unknown Command */
	MGMT_STATUS_NOT_CONNECTED,	/* No Connection */
	MGMT_STATUS_FAILED,		/* Hardware Failure */
	MGMT_STATUS_CONNECT_FAILED,	/* Page Timeout */
	MGMT_STATUS_AUTH_FAILED,	/* Authentication Failed */
	MGMT_STATUS_AUTH_FAILED,	/* PIN or Key Missing */
	MGMT_STATUS_NO_RESOURCES,	/* Memory Full */
	MGMT_STATUS_TIMEOUT,		/* Connection Timeout */
	MGMT_STATUS_NO_RESOURCES,	/* Max Number of Connections */
	MGMT_STATUS_NO_RESOURCES,	/* Max Number of SCO Connections */
	MGMT_STATUS_ALREADY_CONNECTED,	/* ACL Connection Exists */
	MGMT_STATUS_BUSY,		/* Command Disallowed */
	MGMT_STATUS_NO_RESOURCES,	/* Rejected Limited Resources */
	MGMT_STATUS_REJECTED,		/* Rejected Security */
	MGMT_STATUS_REJECTED,		/* Rejected Personal */
	MGMT_STATUS_TIMEOUT,		/* Host Timeout */
	MGMT_STATUS_NOT_SUPPORTED,	/* Unsupported Feature */
	MGMT_STATUS_INVALID_PARAMS,	/* Invalid Parameters */
	MGMT_STATUS_DISCONNECTED,	/* OE User Ended Connection */
	MGMT_STATUS_NO_RESOURCES,	/* OE Low Resources */
	MGMT_STATUS_DISCONNECTED,	/* OE Power Off */
	MGMT_STATUS_DISCONNECTED,	/* Connection Terminated */
	MGMT_STATUS_BUSY,		/* Repeated Attempts */
	MGMT_STATUS_REJECTED,		/* Pairing Not Allowed */
	MGMT_STATUS_FAILED,		/* Unknown LMP PDU */
	MGMT_STATUS_NOT_SUPPORTED,	/* Unsupported Remote Feature */
	MGMT_STATUS_REJECTED,		/* SCO Offset Rejected */
	MGMT_STATUS_REJECTED,		/* SCO Interval Rejected */
	MGMT_STATUS_REJECTED,		/* Air Mode Rejected */
	MGMT_STATUS_INVALID_PARAMS,	/* Invalid LMP Parameters */
	MGMT_STATUS_FAILED,		/* Unspecified Error */
	MGMT_STATUS_NOT_SUPPORTED,	/* Unsupported LMP Parameter Value */
	MGMT_STATUS_FAILED,		/* Role Change Not Allowed */
	MGMT_STATUS_TIMEOUT,		/* LMP Response Timeout */
	MGMT_STATUS_FAILED,		/* LMP Error Transaction Collision */
	MGMT_STATUS_FAILED,		/* LMP PDU Not Allowed */
	MGMT_STATUS_REJECTED,		/* Encryption Mode Not Accepted */
	MGMT_STATUS_FAILED,		/* Unit Link Key Used */
	MGMT_STATUS_NOT_SUPPORTED,	/* QoS Not Supported */
	MGMT_STATUS_TIMEOUT,		/* Instant Passed */
	MGMT_STATUS_NOT_SUPPORTED,	/* Pairing Not Supported */
	MGMT_STATUS_FAILED,		/* Transaction Collision */
	MGMT_STATUS_INVALID_PARAMS,	/* Unacceptable Parameter */
	MGMT_STATUS_REJECTED,		/* QoS Rejected */
	MGMT_STATUS_NOT_SUPPORTED,	/* Classification Not Supported */
	MGMT_STATUS_REJECTED,		/* Insufficient Security */
	MGMT_STATUS_INVALID_PARAMS,	/* Parameter Out Of Range */
	MGMT_STATUS_BUSY,		/* Role Switch Pending */
	MGMT_STATUS_FAILED,		/* Slot Violation */
	MGMT_STATUS_FAILED,		/* Role Switch Failed */
	MGMT_STATUS_INVALID_PARAMS,	/* EIR Too Large */
	MGMT_STATUS_NOT_SUPPORTED,	/* Simple Pairing Not Supported */
	MGMT_STATUS_BUSY,		/* Host Busy Pairing */
	MGMT_STATUS_REJECTED,		/* Rejected, No Suitable Channel */
	MGMT_STATUS_BUSY,		/* Controller Busy */
	MGMT_STATUS_INVALID_PARAMS,	/* Unsuitable Connection Interval */
	MGMT_STATUS_TIMEOUT,		/* Directed Advertising Timeout */
	MGMT_STATUS_AUTH_FAILED,	/* Terminated Due to MIC Failure */
	MGMT_STATUS_CONNECT_FAILED,	/* Connection Establishment Failed */
	MGMT_STATUS_CONNECT_FAILED,	/* MAC Connection Failed */
>>>>>>> android-4.9
};

LIST_HEAD(cmd_list);

static int mgmt_index_event(u16 event, struct hci_dev *hdev, void *data,
			    u16 len, int flag)
{
<<<<<<< HEAD
	struct sk_buff *skb;
	struct mgmt_hdr *hdr;
	struct mgmt_ev_cmd_status *ev;

	BT_DBG("sock %p, index %u, cmd %u, status %u", sk, index, cmd, status);

	skb = alloc_skb(sizeof(*hdr) + sizeof(*ev), GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	hdr = (void *) skb_put(skb, sizeof(*hdr));

	hdr->opcode = cpu_to_le16(MGMT_EV_CMD_STATUS);
	hdr->index = cpu_to_le16(index);
	hdr->len = cpu_to_le16(sizeof(*ev));

	ev = (void *) skb_put(skb, sizeof(*ev));
	ev->status = status;
	put_unaligned_le16(cmd, &ev->opcode);

	if (sock_queue_rcv_skb(sk, skb) < 0)
		kfree_skb(skb);

	return 0;
}

static int cmd_complete(struct sock *sk, u16 index, u16 cmd, void *rp,
								size_t rp_len)
{
	struct sk_buff *skb;
	struct mgmt_hdr *hdr;
	struct mgmt_ev_cmd_complete *ev;

	BT_DBG("sock %p", sk);

	skb = alloc_skb(sizeof(*hdr) + sizeof(*ev) + rp_len, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	hdr = (void *) skb_put(skb, sizeof(*hdr));

	hdr->opcode = cpu_to_le16(MGMT_EV_CMD_COMPLETE);
	hdr->index = cpu_to_le16(index);
	hdr->len = cpu_to_le16(sizeof(*ev) + rp_len);

	ev = (void *) skb_put(skb, sizeof(*ev) + rp_len);
	put_unaligned_le16(cmd, &ev->opcode);
=======
	return mgmt_send_event(event, hdev, HCI_CHANNEL_CONTROL, data, len,
			       flag, NULL);
}

static int mgmt_limited_event(u16 event, struct hci_dev *hdev, void *data,
			      u16 len, int flag, struct sock *skip_sk)
{
	return mgmt_send_event(event, hdev, HCI_CHANNEL_CONTROL, data, len,
			       flag, skip_sk);
}

static int mgmt_event(u16 event, struct hci_dev *hdev, void *data, u16 len,
		      struct sock *skip_sk)
{
	return mgmt_send_event(event, hdev, HCI_CHANNEL_CONTROL, data, len,
			       HCI_SOCK_TRUSTED, skip_sk);
}
>>>>>>> android-4.9

static u8 le_addr_type(u8 mgmt_addr_type)
{
	if (mgmt_addr_type == BDADDR_LE_PUBLIC)
		return ADDR_LE_DEV_PUBLIC;
	else
		return ADDR_LE_DEV_RANDOM;
}

<<<<<<< HEAD
	if (sock_queue_rcv_skb(sk, skb) < 0)
		kfree_skb(skb);

	return 0;
=======
void mgmt_fill_version_info(void *ver)
{
	struct mgmt_rp_read_version *rp = ver;

	rp->version = MGMT_VERSION;
	rp->revision = cpu_to_le16(MGMT_REVISION);
>>>>>>> android-4.9
}

static int read_version(struct sock *sk)
{
	struct mgmt_rp_read_version rp;

	BT_DBG("sock %p", sk);

	mgmt_fill_version_info(&rp);

<<<<<<< HEAD
	return cmd_complete(sk, MGMT_INDEX_NONE, MGMT_OP_READ_VERSION, &rp,
								sizeof(rp));
=======
	return mgmt_cmd_complete(sk, MGMT_INDEX_NONE, MGMT_OP_READ_VERSION, 0,
				 &rp, sizeof(rp));
}

static int read_commands(struct sock *sk, struct hci_dev *hdev, void *data,
			 u16 data_len)
{
	struct mgmt_rp_read_commands *rp;
	u16 num_commands, num_events;
	size_t rp_size;
	int i, err;

	BT_DBG("sock %p", sk);

	if (hci_sock_test_flag(sk, HCI_SOCK_TRUSTED)) {
		num_commands = ARRAY_SIZE(mgmt_commands);
		num_events = ARRAY_SIZE(mgmt_events);
	} else {
		num_commands = ARRAY_SIZE(mgmt_untrusted_commands);
		num_events = ARRAY_SIZE(mgmt_untrusted_events);
	}

	rp_size = sizeof(*rp) + ((num_commands + num_events) * sizeof(u16));

	rp = kmalloc(rp_size, GFP_KERNEL);
	if (!rp)
		return -ENOMEM;

	rp->num_commands = cpu_to_le16(num_commands);
	rp->num_events = cpu_to_le16(num_events);

	if (hci_sock_test_flag(sk, HCI_SOCK_TRUSTED)) {
		__le16 *opcode = rp->opcodes;

		for (i = 0; i < num_commands; i++, opcode++)
			put_unaligned_le16(mgmt_commands[i], opcode);

		for (i = 0; i < num_events; i++, opcode++)
			put_unaligned_le16(mgmt_events[i], opcode);
	} else {
		__le16 *opcode = rp->opcodes;

		for (i = 0; i < num_commands; i++, opcode++)
			put_unaligned_le16(mgmt_untrusted_commands[i], opcode);

		for (i = 0; i < num_events; i++, opcode++)
			put_unaligned_le16(mgmt_untrusted_events[i], opcode);
	}

	err = mgmt_cmd_complete(sk, MGMT_INDEX_NONE, MGMT_OP_READ_COMMANDS, 0,
				rp, rp_size);
	kfree(rp);

	return err;
>>>>>>> android-4.9
}

static int read_index_list(struct sock *sk)
{
	struct mgmt_rp_read_index_list *rp;
<<<<<<< HEAD
	struct list_head *p;
=======
	struct hci_dev *d;
>>>>>>> android-4.9
	size_t rp_len;
	u16 count;
	int err;

	BT_DBG("sock %p", sk);

	read_lock(&hci_dev_list_lock);

	count = 0;
<<<<<<< HEAD
	list_for_each(p, &hci_dev_list) {
		struct hci_dev *d = list_entry(p, struct hci_dev, list);
		if (d->dev_type != HCI_BREDR)
			continue;
		count++;
=======
	list_for_each_entry(d, &hci_dev_list, list) {
		if (d->dev_type == HCI_PRIMARY &&
		    !hci_dev_test_flag(d, HCI_UNCONFIGURED))
			count++;
>>>>>>> android-4.9
	}

	rp_len = sizeof(*rp) + (2 * count);
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp) {
		read_unlock(&hci_dev_list_lock);
		return -ENOMEM;
	}

<<<<<<< HEAD
	put_unaligned_le16(0, &rp->num_controllers);

	i = 0;
	list_for_each(p, &hci_dev_list) {
		struct hci_dev *d = list_entry(p, struct hci_dev, list);

		hci_del_off_timer(d);

		if (d->dev_type != HCI_BREDR)
			continue;

		set_bit(HCI_MGMT, &d->flags);

		if (test_bit(HCI_SETUP, &d->flags))
			continue;

		put_unaligned_le16(d->id, &rp->index[i++]);
		put_unaligned_le16((u16)i, &rp->num_controllers);
		BT_DBG("Added hci%u", d->id);
=======
	count = 0;
	list_for_each_entry(d, &hci_dev_list, list) {
		if (hci_dev_test_flag(d, HCI_SETUP) ||
		    hci_dev_test_flag(d, HCI_CONFIG) ||
		    hci_dev_test_flag(d, HCI_USER_CHANNEL))
			continue;

		/* Devices marked as raw-only are neither configured
		 * nor unconfigured controllers.
		 */
		if (test_bit(HCI_QUIRK_RAW_DEVICE, &d->quirks))
			continue;

		if (d->dev_type == HCI_PRIMARY &&
		    !hci_dev_test_flag(d, HCI_UNCONFIGURED)) {
			rp->index[count++] = cpu_to_le16(d->id);
			BT_DBG("Added hci%u", d->id);
		}
>>>>>>> android-4.9
	}

	rp->num_controllers = cpu_to_le16(count);
	rp_len = sizeof(*rp) + (2 * count);

	read_unlock(&hci_dev_list_lock);

<<<<<<< HEAD
	err = cmd_complete(sk, MGMT_INDEX_NONE, MGMT_OP_READ_INDEX_LIST, rp,
									rp_len);
=======
	err = mgmt_cmd_complete(sk, MGMT_INDEX_NONE, MGMT_OP_READ_INDEX_LIST,
				0, rp, rp_len);
>>>>>>> android-4.9

	kfree(rp);

	return err;
}

<<<<<<< HEAD
static int read_controller_info(struct sock *sk, u16 index)
{
	struct mgmt_rp_read_info rp;
	struct hci_dev *hdev;

	BT_DBG("sock %p hci%u", sk, index);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_READ_INFO, ENODEV);

	hci_del_off_timer(hdev);

	hci_dev_lock_bh(hdev);

	set_bit(HCI_MGMT, &hdev->flags);

	memset(&rp, 0, sizeof(rp));

	rp.type = hdev->dev_type;

	rp.powered = test_bit(HCI_UP, &hdev->flags);
	rp.connectable = test_bit(HCI_PSCAN, &hdev->flags);
	rp.discoverable = test_bit(HCI_ISCAN, &hdev->flags);
	rp.pairable = test_bit(HCI_PSCAN, &hdev->flags);

	if (test_bit(HCI_AUTH, &hdev->flags))
		rp.sec_mode = 3;
	else if (hdev->ssp_mode > 0)
		rp.sec_mode = 4;
	else
		rp.sec_mode = 2;

	bacpy(&rp.bdaddr, &hdev->bdaddr);
	memcpy(rp.features, hdev->features, 8);
	memcpy(rp.dev_class, hdev->dev_class, 3);
	put_unaligned_le16(hdev->manufacturer, &rp.manufacturer);
	rp.hci_ver = hdev->hci_ver;
	put_unaligned_le16(hdev->hci_rev, &rp.hci_rev);

	memcpy(rp.name, hdev->dev_name, sizeof(hdev->dev_name));

	rp.le_white_list_size = hdev->le_white_list_size;

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return cmd_complete(sk, index, MGMT_OP_READ_INFO, &rp, sizeof(rp));
}

static void mgmt_pending_free_worker(struct work_struct *work)
{
	struct mgmt_pending_free_work *free_work =
		container_of(work, struct mgmt_pending_free_work, work);

	BT_DBG("sk %p", free_work->sk);

	sock_put(free_work->sk);
	kfree(free_work);
}

static void mgmt_pending_free(struct pending_cmd *cmd)
{
	struct mgmt_pending_free_work *free_work;
	struct sock *sk = cmd->sk;

	BT_DBG("opcode %d, sk %p", cmd->opcode, sk);

	kfree(cmd->param);
	kfree(cmd);

	free_work = kzalloc(sizeof(*free_work), GFP_ATOMIC);
	if (free_work) {
		INIT_WORK(&free_work->work, mgmt_pending_free_worker);
		free_work->sk = sk;

		if (!schedule_work(&free_work->work))
			kfree(free_work);
	}
}

static struct pending_cmd *mgmt_pending_add(struct sock *sk, u16 opcode,
						u16 index, void *data, u16 len)
{
	struct pending_cmd *cmd;

	BT_DBG("%d", opcode);

	cmd = kmalloc(sizeof(*cmd), GFP_ATOMIC);
	if (!cmd)
		return NULL;

	cmd->opcode = opcode;
	cmd->index = index;

	cmd->param = kmalloc(len, GFP_ATOMIC);
	if (!cmd->param) {
		kfree(cmd);
		return NULL;
	}

	if (data)
		memcpy(cmd->param, data, len);

	cmd->sk = sk;
	sock_hold(sk);

	list_add(&cmd->list, &cmd_list);

	return cmd;
}

static void mgmt_pending_foreach(u16 opcode, int index,
				void (*cb)(struct pending_cmd *cmd, void *data),
				void *data)
{
	struct list_head *p, *n;

	BT_DBG(" %d", opcode);

	list_for_each_safe(p, n, &cmd_list) {
		struct pending_cmd *cmd;

		cmd = list_entry(p, struct pending_cmd, list);

		if (opcode > 0 && cmd->opcode != opcode)
			continue;

		if (index >= 0 && cmd->index != index)
			continue;

		cb(cmd, data);
	}
}

static struct pending_cmd *mgmt_pending_find(u16 opcode, int index)
{
	struct list_head *p;

	BT_DBG(" %d", opcode);

	list_for_each(p, &cmd_list) {
		struct pending_cmd *cmd;

		cmd = list_entry(p, struct pending_cmd, list);

		if (cmd->opcode != opcode)
			continue;

		if (index >= 0 && cmd->index != index)
			continue;

		return cmd;
	}

	return NULL;
}

static void mgmt_pending_remove(struct pending_cmd *cmd)
{
	BT_DBG(" %d", cmd->opcode);

	list_del(&cmd->list);
	mgmt_pending_free(cmd);
}

static int set_powered(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct mgmt_mode *cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	int err, up;

	cp = (void *) data;

	BT_DBG("request for hci%u", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_POWERED, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_POWERED, ENODEV);

	hci_dev_lock_bh(hdev);

	up = test_bit(HCI_UP, &hdev->flags);
	if ((cp->val && up) || (!cp->val && !up)) {
		err = cmd_status(sk, index, MGMT_OP_SET_POWERED, EALREADY);
		goto failed;
	}

	if (mgmt_pending_find(MGMT_OP_SET_POWERED, index)) {
		err = cmd_status(sk, index, MGMT_OP_SET_POWERED, EBUSY);
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_POWERED, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	hci_dev_unlock_bh(hdev);

	if (cp->val)
		queue_work(hdev->workqueue, &hdev->power_on);
	else
		queue_work(hdev->workqueue, &hdev->power_off);

	err = 0;
	hci_dev_put(hdev);

	return err;

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
	return err;
}

static u8 get_service_classes(struct hci_dev *hdev)
{
	struct list_head *p;
	u8 val = 0;

	list_for_each(p, &hdev->uuids) {
		struct bt_uuid *uuid = list_entry(p, struct bt_uuid, list);

		val |= uuid->svc_hint;
=======
static int read_unconf_index_list(struct sock *sk, struct hci_dev *hdev,
				  void *data, u16 data_len)
{
	struct mgmt_rp_read_unconf_index_list *rp;
	struct hci_dev *d;
	size_t rp_len;
	u16 count;
	int err;

	BT_DBG("sock %p", sk);

	read_lock(&hci_dev_list_lock);

	count = 0;
	list_for_each_entry(d, &hci_dev_list, list) {
		if (d->dev_type == HCI_PRIMARY &&
		    hci_dev_test_flag(d, HCI_UNCONFIGURED))
			count++;
	}

	rp_len = sizeof(*rp) + (2 * count);
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp) {
		read_unlock(&hci_dev_list_lock);
		return -ENOMEM;
	}

	count = 0;
	list_for_each_entry(d, &hci_dev_list, list) {
		if (hci_dev_test_flag(d, HCI_SETUP) ||
		    hci_dev_test_flag(d, HCI_CONFIG) ||
		    hci_dev_test_flag(d, HCI_USER_CHANNEL))
			continue;

		/* Devices marked as raw-only are neither configured
		 * nor unconfigured controllers.
		 */
		if (test_bit(HCI_QUIRK_RAW_DEVICE, &d->quirks))
			continue;

		if (d->dev_type == HCI_PRIMARY &&
		    hci_dev_test_flag(d, HCI_UNCONFIGURED)) {
			rp->index[count++] = cpu_to_le16(d->id);
			BT_DBG("Added hci%u", d->id);
		}
	}

	rp->num_controllers = cpu_to_le16(count);
	rp_len = sizeof(*rp) + (2 * count);

	read_unlock(&hci_dev_list_lock);

	err = mgmt_cmd_complete(sk, MGMT_INDEX_NONE,
				MGMT_OP_READ_UNCONF_INDEX_LIST, 0, rp, rp_len);

	kfree(rp);

	return err;
}

static int read_ext_index_list(struct sock *sk, struct hci_dev *hdev,
			       void *data, u16 data_len)
{
	struct mgmt_rp_read_ext_index_list *rp;
	struct hci_dev *d;
	size_t rp_len;
	u16 count;
	int err;

	BT_DBG("sock %p", sk);

	read_lock(&hci_dev_list_lock);

	count = 0;
	list_for_each_entry(d, &hci_dev_list, list) {
		if (d->dev_type == HCI_PRIMARY || d->dev_type == HCI_AMP)
			count++;
	}

	rp_len = sizeof(*rp) + (sizeof(rp->entry[0]) * count);
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp) {
		read_unlock(&hci_dev_list_lock);
		return -ENOMEM;
	}

	count = 0;
	list_for_each_entry(d, &hci_dev_list, list) {
		if (hci_dev_test_flag(d, HCI_SETUP) ||
		    hci_dev_test_flag(d, HCI_CONFIG) ||
		    hci_dev_test_flag(d, HCI_USER_CHANNEL))
			continue;

		/* Devices marked as raw-only are neither configured
		 * nor unconfigured controllers.
		 */
		if (test_bit(HCI_QUIRK_RAW_DEVICE, &d->quirks))
			continue;

		if (d->dev_type == HCI_PRIMARY) {
			if (hci_dev_test_flag(d, HCI_UNCONFIGURED))
				rp->entry[count].type = 0x01;
			else
				rp->entry[count].type = 0x00;
		} else if (d->dev_type == HCI_AMP) {
			rp->entry[count].type = 0x02;
		} else {
			continue;
		}

		rp->entry[count].bus = d->bus;
		rp->entry[count++].index = cpu_to_le16(d->id);
		BT_DBG("Added hci%u", d->id);
	}

	rp->num_controllers = cpu_to_le16(count);
	rp_len = sizeof(*rp) + (sizeof(rp->entry[0]) * count);

	read_unlock(&hci_dev_list_lock);

	/* If this command is called at least once, then all the
	 * default index and unconfigured index events are disabled
	 * and from now on only extended index events are used.
	 */
	hci_sock_set_flag(sk, HCI_MGMT_EXT_INDEX_EVENTS);
	hci_sock_clear_flag(sk, HCI_MGMT_INDEX_EVENTS);
	hci_sock_clear_flag(sk, HCI_MGMT_UNCONF_INDEX_EVENTS);

	err = mgmt_cmd_complete(sk, MGMT_INDEX_NONE,
				MGMT_OP_READ_EXT_INDEX_LIST, 0, rp, rp_len);

	kfree(rp);

	return err;
}

static bool is_configured(struct hci_dev *hdev)
{
	if (test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks) &&
	    !hci_dev_test_flag(hdev, HCI_EXT_CONFIGURED))
		return false;

	if (test_bit(HCI_QUIRK_INVALID_BDADDR, &hdev->quirks) &&
	    !bacmp(&hdev->public_addr, BDADDR_ANY))
		return false;

	return true;
}

static __le32 get_missing_options(struct hci_dev *hdev)
{
	u32 options = 0;

	if (test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks) &&
	    !hci_dev_test_flag(hdev, HCI_EXT_CONFIGURED))
		options |= MGMT_OPTION_EXTERNAL_CONFIG;

	if (test_bit(HCI_QUIRK_INVALID_BDADDR, &hdev->quirks) &&
	    !bacmp(&hdev->public_addr, BDADDR_ANY))
		options |= MGMT_OPTION_PUBLIC_ADDRESS;

	return cpu_to_le32(options);
}

static int new_options(struct hci_dev *hdev, struct sock *skip)
{
	__le32 options = get_missing_options(hdev);

	return mgmt_limited_event(MGMT_EV_NEW_CONFIG_OPTIONS, hdev, &options,
				  sizeof(options), HCI_MGMT_OPTION_EVENTS, skip);
}

static int send_options_rsp(struct sock *sk, u16 opcode, struct hci_dev *hdev)
{
	__le32 options = get_missing_options(hdev);

	return mgmt_cmd_complete(sk, hdev->id, opcode, 0, &options,
				 sizeof(options));
}

static int read_config_info(struct sock *sk, struct hci_dev *hdev,
			    void *data, u16 data_len)
{
	struct mgmt_rp_read_config_info rp;
	u32 options = 0;

	BT_DBG("sock %p %s", sk, hdev->name);

	hci_dev_lock(hdev);

	memset(&rp, 0, sizeof(rp));
	rp.manufacturer = cpu_to_le16(hdev->manufacturer);

	if (test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks))
		options |= MGMT_OPTION_EXTERNAL_CONFIG;

	if (hdev->set_bdaddr)
		options |= MGMT_OPTION_PUBLIC_ADDRESS;

	rp.supported_options = cpu_to_le32(options);
	rp.missing_options = get_missing_options(hdev);

	hci_dev_unlock(hdev);

	return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_READ_CONFIG_INFO, 0,
				 &rp, sizeof(rp));
}

static u32 get_supported_settings(struct hci_dev *hdev)
{
	u32 settings = 0;

	settings |= MGMT_SETTING_POWERED;
	settings |= MGMT_SETTING_BONDABLE;
	settings |= MGMT_SETTING_DEBUG_KEYS;
	settings |= MGMT_SETTING_CONNECTABLE;
	settings |= MGMT_SETTING_DISCOVERABLE;

	if (lmp_bredr_capable(hdev)) {
		if (hdev->hci_ver >= BLUETOOTH_VER_1_2)
			settings |= MGMT_SETTING_FAST_CONNECTABLE;
		settings |= MGMT_SETTING_BREDR;
		settings |= MGMT_SETTING_LINK_SECURITY;

		if (lmp_ssp_capable(hdev)) {
			settings |= MGMT_SETTING_SSP;
			settings |= MGMT_SETTING_HS;
		}

		if (lmp_sc_capable(hdev))
			settings |= MGMT_SETTING_SECURE_CONN;
	}

	if (lmp_le_capable(hdev)) {
		settings |= MGMT_SETTING_LE;
		settings |= MGMT_SETTING_ADVERTISING;
		settings |= MGMT_SETTING_SECURE_CONN;
		settings |= MGMT_SETTING_PRIVACY;
		settings |= MGMT_SETTING_STATIC_ADDRESS;
	}

	if (test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks) ||
	    hdev->set_bdaddr)
		settings |= MGMT_SETTING_CONFIGURATION;

	return settings;
}

static u32 get_current_settings(struct hci_dev *hdev)
{
	u32 settings = 0;

	if (hdev_is_powered(hdev))
		settings |= MGMT_SETTING_POWERED;

	if (hci_dev_test_flag(hdev, HCI_CONNECTABLE))
		settings |= MGMT_SETTING_CONNECTABLE;

	if (hci_dev_test_flag(hdev, HCI_FAST_CONNECTABLE))
		settings |= MGMT_SETTING_FAST_CONNECTABLE;

	if (hci_dev_test_flag(hdev, HCI_DISCOVERABLE))
		settings |= MGMT_SETTING_DISCOVERABLE;

	if (hci_dev_test_flag(hdev, HCI_BONDABLE))
		settings |= MGMT_SETTING_BONDABLE;

	if (hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		settings |= MGMT_SETTING_BREDR;

	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		settings |= MGMT_SETTING_LE;

	if (hci_dev_test_flag(hdev, HCI_LINK_SECURITY))
		settings |= MGMT_SETTING_LINK_SECURITY;

	if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED))
		settings |= MGMT_SETTING_SSP;

	if (hci_dev_test_flag(hdev, HCI_HS_ENABLED))
		settings |= MGMT_SETTING_HS;

	if (hci_dev_test_flag(hdev, HCI_ADVERTISING))
		settings |= MGMT_SETTING_ADVERTISING;

	if (hci_dev_test_flag(hdev, HCI_SC_ENABLED))
		settings |= MGMT_SETTING_SECURE_CONN;

	if (hci_dev_test_flag(hdev, HCI_KEEP_DEBUG_KEYS))
		settings |= MGMT_SETTING_DEBUG_KEYS;

	if (hci_dev_test_flag(hdev, HCI_PRIVACY))
		settings |= MGMT_SETTING_PRIVACY;

	/* The current setting for static address has two purposes. The
	 * first is to indicate if the static address will be used and
	 * the second is to indicate if it is actually set.
	 *
	 * This means if the static address is not configured, this flag
	 * will never be set. If the address is configured, then if the
	 * address is actually used decides if the flag is set or not.
	 *
	 * For single mode LE only controllers and dual-mode controllers
	 * with BR/EDR disabled, the existence of the static address will
	 * be evaluated.
	 */
	if (hci_dev_test_flag(hdev, HCI_FORCE_STATIC_ADDR) ||
	    !hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) ||
	    !bacmp(&hdev->bdaddr, BDADDR_ANY)) {
		if (bacmp(&hdev->static_addr, BDADDR_ANY))
			settings |= MGMT_SETTING_STATIC_ADDRESS;
>>>>>>> android-4.9
	}

	return settings;
}

static struct mgmt_pending_cmd *pending_find(u16 opcode, struct hci_dev *hdev)
{
<<<<<<< HEAD
	u8 cod[3];
	int err = 0;
=======
	return mgmt_pending_find(HCI_CHANNEL_CONTROL, opcode, hdev);
}
>>>>>>> android-4.9

static struct mgmt_pending_cmd *pending_find_data(u16 opcode,
						  struct hci_dev *hdev,
						  const void *data)
{
	return mgmt_pending_find_data(HCI_CHANNEL_CONTROL, opcode, hdev, data);
}

<<<<<<< HEAD
	if (test_bit(HCI_SERVICE_CACHE, &hdev->flags))
		return 0;
=======
u8 mgmt_get_adv_discov_flags(struct hci_dev *hdev)
{
	struct mgmt_pending_cmd *cmd;

	/* If there's a pending mgmt command the flags will not yet have
	 * their final values, so check for this first.
	 */
	cmd = pending_find(MGMT_OP_SET_DISCOVERABLE, hdev);
	if (cmd) {
		struct mgmt_mode *cp = cmd->param;
		if (cp->val == 0x01)
			return LE_AD_GENERAL;
		else if (cp->val == 0x02)
			return LE_AD_LIMITED;
	} else {
		if (hci_dev_test_flag(hdev, HCI_LIMITED_DISCOVERABLE))
			return LE_AD_LIMITED;
		else if (hci_dev_test_flag(hdev, HCI_DISCOVERABLE))
			return LE_AD_GENERAL;
	}

	return 0;
}
>>>>>>> android-4.9

bool mgmt_get_connectable(struct hci_dev *hdev)
{
	struct mgmt_pending_cmd *cmd;

	/* If there's a pending mgmt command the flag will not yet have
	 * it's final value, so check for this first.
	 */
	cmd = pending_find(MGMT_OP_SET_CONNECTABLE, hdev);
	if (cmd) {
		struct mgmt_mode *cp = cmd->param;

<<<<<<< HEAD
	err =  hci_send_cmd(hdev, HCI_OP_WRITE_CLASS_OF_DEV, sizeof(cod), cod);

	if (err == 0)
		memcpy(hdev->dev_class, cod, 3);
=======
		return cp->val;
	}
>>>>>>> android-4.9

	return hci_dev_test_flag(hdev, HCI_CONNECTABLE);
}

static int set_limited_discoverable(struct sock *sk, u16 index,
						unsigned char *data, u16 len)
{
<<<<<<< HEAD
	struct mgmt_mode *cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	struct hci_cp_write_current_iac_lap dcp;
	int update_cod;
	int err = 0;
	/* General Inquiry LAP: 0x9E8B33, Limited Inquiry LAP: 0x9E8B00 */
	u8 lap[] = { 0x33, 0x8b, 0x9e, 0x00, 0x8b, 0x9e };

	cp = (void *) data;

	BT_DBG("hci%u discoverable: %d", index, cp->val);

	if (!cp || len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_LIMIT_DISCOVERABLE,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_LIMIT_DISCOVERABLE,
									ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_LIMIT_DISCOVERABLE,
								ENETDOWN);
		goto failed;
	}
=======
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    service_cache.work);
	struct hci_request req;

	if (!hci_dev_test_and_clear_flag(hdev, HCI_SERVICE_CACHE))
		return;

	hci_req_init(&req, hdev);

	hci_dev_lock(hdev);

	__hci_req_update_eir(&req);
	__hci_req_update_class(&req);

	hci_dev_unlock(hdev);

	hci_req_run(&req, NULL);
}

static void rpa_expired(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    rpa_expired.work);
	struct hci_request req;

	BT_DBG("");

	hci_dev_set_flag(hdev, HCI_RPA_EXPIRED);

	if (!hci_dev_test_flag(hdev, HCI_ADVERTISING))
		return;

	/* The generation of a new RPA and programming it into the
	 * controller happens in the hci_req_enable_advertising()
	 * function.
	 */
	hci_req_init(&req, hdev);
	__hci_req_enable_advertising(&req);
	hci_req_run(&req, NULL);
}

static void mgmt_init_hdev(struct sock *sk, struct hci_dev *hdev)
{
	if (hci_dev_test_and_set_flag(hdev, HCI_MGMT))
		return;

	INIT_DELAYED_WORK(&hdev->service_cache, service_cache_off);
	INIT_DELAYED_WORK(&hdev->rpa_expired, rpa_expired);

	/* Non-mgmt controlled devices get this bit set
	 * implicitly so that pairing works for them, however
	 * for mgmt we require user-space to explicitly enable
	 * it
	 */
	hci_dev_clear_flag(hdev, HCI_BONDABLE);
}
>>>>>>> android-4.9

	if (mgmt_pending_find(MGMT_OP_SET_LIMIT_DISCOVERABLE, index)) {
		err = cmd_status(sk, index, MGMT_OP_SET_LIMIT_DISCOVERABLE,
									EBUSY);
		goto failed;
	}

	if (cp->val == test_bit(HCI_ISCAN, &hdev->flags) &&
					test_bit(HCI_PSCAN, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_LIMIT_DISCOVERABLE,
								EALREADY);
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_LIMIT_DISCOVERABLE, index, data,
									len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	memset(&dcp, 0, sizeof(dcp));
	dcp.num_current_iac = cp->val ? 2 : 1;
	memcpy(&dcp.lap, lap, dcp.num_current_iac * 3);
	update_cod = 1;

	if (cp->val) {
		if (hdev->major_class & MGMT_MAJOR_CLASS_LIMITED)
			update_cod = 0;
		hdev->major_class |= MGMT_MAJOR_CLASS_LIMITED;
	} else {
		if (!(hdev->major_class & MGMT_MAJOR_CLASS_LIMITED))
			update_cod = 0;
		hdev->major_class &= ~MGMT_MAJOR_CLASS_LIMITED;
	}

<<<<<<< HEAD
	if (update_cod)
		err = update_class(hdev);

	if (err >= 0)
		err = hci_send_cmd(hdev, HCI_OP_WRITE_CURRENT_IAC_LAP,
							sizeof(dcp), &dcp);
=======
	rp.version = hdev->hci_ver;
	rp.manufacturer = cpu_to_le16(hdev->manufacturer);
>>>>>>> android-4.9

	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int set_discoverable(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct mgmt_mode *cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	u8 scan;
=======
	hci_dev_unlock(hdev);

	return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_READ_INFO, 0, &rp,
				 sizeof(rp));
}

static u16 append_eir_data_to_buf(struct hci_dev *hdev, u8 *eir)
{
	u16 eir_len = 0;
	size_t name_len;

	if (hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		eir_len = eir_append_data(eir, eir_len, EIR_CLASS_OF_DEV,
					  hdev->dev_class, 3);

	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		eir_len = eir_append_le16(eir, eir_len, EIR_APPEARANCE,
					  hdev->appearance);

	name_len = strlen(hdev->dev_name);
	eir_len = eir_append_data(eir, eir_len, EIR_NAME_COMPLETE,
				  hdev->dev_name, name_len);

	name_len = strlen(hdev->short_name);
	eir_len = eir_append_data(eir, eir_len, EIR_NAME_SHORT,
				  hdev->short_name, name_len);

	return eir_len;
}

static int read_ext_controller_info(struct sock *sk, struct hci_dev *hdev,
				    void *data, u16 data_len)
{
	char buf[512];
	struct mgmt_rp_read_ext_info *rp = (void *)buf;
	u16 eir_len;

	BT_DBG("sock %p %s", sk, hdev->name);

	memset(&buf, 0, sizeof(buf));

	hci_dev_lock(hdev);

	bacpy(&rp->bdaddr, &hdev->bdaddr);

	rp->version = hdev->hci_ver;
	rp->manufacturer = cpu_to_le16(hdev->manufacturer);

	rp->supported_settings = cpu_to_le32(get_supported_settings(hdev));
	rp->current_settings = cpu_to_le32(get_current_settings(hdev));


	eir_len = append_eir_data_to_buf(hdev, rp->eir);
	rp->eir_len = cpu_to_le16(eir_len);

	hci_dev_unlock(hdev);

	/* If this command is called at least once, then the events
	 * for class of device and local name changes are disabled
	 * and only the new extended controller information event
	 * is used.
	 */
	hci_sock_set_flag(sk, HCI_MGMT_EXT_INFO_EVENTS);
	hci_sock_clear_flag(sk, HCI_MGMT_DEV_CLASS_EVENTS);
	hci_sock_clear_flag(sk, HCI_MGMT_LOCAL_NAME_EVENTS);

	return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_READ_EXT_INFO, 0, rp,
				 sizeof(*rp) + eir_len);
}

static int ext_info_changed(struct hci_dev *hdev, struct sock *skip)
{
	char buf[512];
	struct mgmt_ev_ext_info_changed *ev = (void *)buf;
	u16 eir_len;

	memset(buf, 0, sizeof(buf));

	eir_len = append_eir_data_to_buf(hdev, ev->eir);
	ev->eir_len = cpu_to_le16(eir_len);

	return mgmt_limited_event(MGMT_EV_EXT_INFO_CHANGED, hdev, ev,
				  sizeof(*ev) + eir_len,
				  HCI_MGMT_EXT_INFO_EVENTS, skip);
}

static int send_settings_rsp(struct sock *sk, u16 opcode, struct hci_dev *hdev)
{
	__le32 settings = cpu_to_le32(get_current_settings(hdev));

	return mgmt_cmd_complete(sk, hdev->id, opcode, 0, &settings,
				 sizeof(settings));
}

static void clean_up_hci_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	BT_DBG("%s status 0x%02x", hdev->name, status);

	if (hci_conn_count(hdev) == 0) {
		cancel_delayed_work(&hdev->power_off);
		queue_work(hdev->req_workqueue, &hdev->power_off.work);
	}
}

void mgmt_advertising_added(struct sock *sk, struct hci_dev *hdev, u8 instance)
{
	struct mgmt_ev_advertising_added ev;

	ev.instance = instance;

	mgmt_event(MGMT_EV_ADVERTISING_ADDED, hdev, &ev, sizeof(ev), sk);
}

void mgmt_advertising_removed(struct sock *sk, struct hci_dev *hdev,
			      u8 instance)
{
	struct mgmt_ev_advertising_removed ev;

	ev.instance = instance;

	mgmt_event(MGMT_EV_ADVERTISING_REMOVED, hdev, &ev, sizeof(ev), sk);
}

static void cancel_adv_timeout(struct hci_dev *hdev)
{
	if (hdev->adv_instance_timeout) {
		hdev->adv_instance_timeout = 0;
		cancel_delayed_work(&hdev->adv_instance_expire);
	}
}

static int clean_up_hci_state(struct hci_dev *hdev)
{
	struct hci_request req;
	struct hci_conn *conn;
	bool discov_stopped;
	int err;

	hci_req_init(&req, hdev);

	if (test_bit(HCI_ISCAN, &hdev->flags) ||
	    test_bit(HCI_PSCAN, &hdev->flags)) {
		u8 scan = 0x00;
		hci_req_add(&req, HCI_OP_WRITE_SCAN_ENABLE, 1, &scan);
	}

	hci_req_clear_adv_instance(hdev, NULL, NULL, 0x00, false);

	if (hci_dev_test_flag(hdev, HCI_LE_ADV))
		__hci_req_disable_advertising(&req);

	discov_stopped = hci_req_stop_discovery(&req);

	list_for_each_entry(conn, &hdev->conn_hash.list, list) {
		/* 0x15 == Terminated due to Power Off */
		__hci_abort_conn(&req, conn, 0x15);
	}

	err = hci_req_run(&req, clean_up_hci_complete);
	if (!err && discov_stopped)
		hci_discovery_set_state(hdev, DISCOVERY_STOPPING);

	return err;
}

static int set_powered(struct sock *sk, struct hci_dev *hdev, void *data,
		       u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_POWERED,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (pending_find(MGMT_OP_SET_POWERED, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_POWERED,
				      MGMT_STATUS_BUSY);
		goto failed;
	}

	if (!!cp->val == hdev_is_powered(hdev)) {
		err = send_settings_rsp(sk, MGMT_OP_SET_POWERED, hdev);
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_POWERED, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	if (cp->val) {
		queue_work(hdev->req_workqueue, &hdev->power_on);
		err = 0;
	} else {
		/* Disconnect connections, stop scans, etc */
		err = clean_up_hci_state(hdev);
		if (!err)
			queue_delayed_work(hdev->req_workqueue, &hdev->power_off,
					   HCI_POWER_OFF_TIMEOUT);

		/* ENODATA means there were no HCI commands queued */
		if (err == -ENODATA) {
			cancel_delayed_work(&hdev->power_off);
			queue_work(hdev->req_workqueue, &hdev->power_off.work);
			err = 0;
		}
	}

failed:
	hci_dev_unlock(hdev);
	return err;
}

static int new_settings(struct hci_dev *hdev, struct sock *skip)
{
	__le32 ev = cpu_to_le32(get_current_settings(hdev));

	return mgmt_limited_event(MGMT_EV_NEW_SETTINGS, hdev, &ev,
				  sizeof(ev), HCI_MGMT_SETTING_EVENTS, skip);
}

int mgmt_new_settings(struct hci_dev *hdev)
{
	return new_settings(hdev, NULL);
}

struct cmd_lookup {
	struct sock *sk;
	struct hci_dev *hdev;
	u8 mgmt_status;
};

static void settings_rsp(struct mgmt_pending_cmd *cmd, void *data)
{
	struct cmd_lookup *match = data;

	send_settings_rsp(cmd->sk, cmd->opcode, match->hdev);

	list_del(&cmd->list);

	if (match->sk == NULL) {
		match->sk = cmd->sk;
		sock_hold(match->sk);
	}

	mgmt_pending_free(cmd);
}

static void cmd_status_rsp(struct mgmt_pending_cmd *cmd, void *data)
{
	u8 *status = data;

	mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode, *status);
	mgmt_pending_remove(cmd);
}

static void cmd_complete_rsp(struct mgmt_pending_cmd *cmd, void *data)
{
	if (cmd->cmd_complete) {
		u8 *status = data;

		cmd->cmd_complete(cmd, *status);
		mgmt_pending_remove(cmd);

		return;
	}

	cmd_status_rsp(cmd, data);
}

static int generic_cmd_complete(struct mgmt_pending_cmd *cmd, u8 status)
{
	return mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode, status,
				 cmd->param, cmd->param_len);
}

static int addr_cmd_complete(struct mgmt_pending_cmd *cmd, u8 status)
{
	return mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode, status,
				 cmd->param, sizeof(struct mgmt_addr_info));
}

static u8 mgmt_bredr_support(struct hci_dev *hdev)
{
	if (!lmp_bredr_capable(hdev))
		return MGMT_STATUS_NOT_SUPPORTED;
	else if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		return MGMT_STATUS_REJECTED;
	else
		return MGMT_STATUS_SUCCESS;
}

static u8 mgmt_le_support(struct hci_dev *hdev)
{
	if (!lmp_le_capable(hdev))
		return MGMT_STATUS_NOT_SUPPORTED;
	else if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return MGMT_STATUS_REJECTED;
	else
		return MGMT_STATUS_SUCCESS;
}

void mgmt_set_discoverable_complete(struct hci_dev *hdev, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status 0x%02x", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_DISCOVERABLE, hdev);
	if (!cmd)
		goto unlock;

	if (status) {
		u8 mgmt_err = mgmt_status(status);
		mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode, mgmt_err);
		hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);
		goto remove_cmd;
	}

	if (hci_dev_test_flag(hdev, HCI_DISCOVERABLE) &&
	    hdev->discov_timeout > 0) {
		int to = msecs_to_jiffies(hdev->discov_timeout * 1000);
		queue_delayed_work(hdev->req_workqueue, &hdev->discov_off, to);
	}

	send_settings_rsp(cmd->sk, MGMT_OP_SET_DISCOVERABLE, hdev);
	new_settings(hdev, cmd->sk);

remove_cmd:
	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int set_discoverable(struct sock *sk, struct hci_dev *hdev, void *data,
			    u16 len)
{
	struct mgmt_cp_set_discoverable *cp = data;
	struct mgmt_pending_cmd *cmd;
	u16 timeout;
>>>>>>> android-4.9
	int err;

	cp = (void *) data;

<<<<<<< HEAD
	BT_DBG("request for hci%u", index);
=======
	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED) &&
	    !hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				       MGMT_STATUS_REJECTED);

	if (cp->val != 0x00 && cp->val != 0x01 && cp->val != 0x02)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				       MGMT_STATUS_INVALID_PARAMS);

	timeout = __le16_to_cpu(cp->timeout);

	/* Disabling discoverable requires that no timeout is set,
	 * and enabling limited discoverable requires a timeout.
	 */
	if ((cp->val == 0x00 && timeout > 0) ||
	    (cp->val == 0x02 && timeout == 0))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				       MGMT_STATUS_INVALID_PARAMS);
>>>>>>> android-4.9

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_DISCOVERABLE, EINVAL);

<<<<<<< HEAD
	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_DISCOVERABLE, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_DISCOVERABLE, ENETDOWN);
		goto failed;
	}

	if (mgmt_pending_find(MGMT_OP_SET_DISCOVERABLE, index) ||
			mgmt_pending_find(MGMT_OP_SET_CONNECTABLE, index)) {
		err = cmd_status(sk, index, MGMT_OP_SET_DISCOVERABLE, EBUSY);
		goto failed;
	}

	if (cp->val == test_bit(HCI_ISCAN, &hdev->flags) &&
					test_bit(HCI_PSCAN, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_DISCOVERABLE, EALREADY);
=======
	if (!hdev_is_powered(hdev) && timeout > 0) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				      MGMT_STATUS_NOT_POWERED);
		goto failed;
	}

	if (pending_find(MGMT_OP_SET_DISCOVERABLE, hdev) ||
	    pending_find(MGMT_OP_SET_CONNECTABLE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				      MGMT_STATUS_BUSY);
		goto failed;
	}

	if (!hci_dev_test_flag(hdev, HCI_CONNECTABLE)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DISCOVERABLE,
				      MGMT_STATUS_REJECTED);
		goto failed;
	}

	if (!hdev_is_powered(hdev)) {
		bool changed = false;

		/* Setting limited discoverable when powered off is
		 * not a valid operation since it requires a timeout
		 * and so no need to check HCI_LIMITED_DISCOVERABLE.
		 */
		if (!!cp->val != hci_dev_test_flag(hdev, HCI_DISCOVERABLE)) {
			hci_dev_change_flag(hdev, HCI_DISCOVERABLE);
			changed = true;
		}

		err = send_settings_rsp(sk, MGMT_OP_SET_DISCOVERABLE, hdev);
		if (err < 0)
			goto failed;

		if (changed)
			err = new_settings(hdev, sk);

		goto failed;
	}

	/* If the current mode is the same, then just update the timeout
	 * value with the new value. And if only the timeout gets updated,
	 * then no need for any HCI transactions.
	 */
	if (!!cp->val == hci_dev_test_flag(hdev, HCI_DISCOVERABLE) &&
	    (cp->val == 0x02) == hci_dev_test_flag(hdev,
						   HCI_LIMITED_DISCOVERABLE)) {
		cancel_delayed_work(&hdev->discov_off);
		hdev->discov_timeout = timeout;

		if (cp->val && hdev->discov_timeout > 0) {
			int to = msecs_to_jiffies(hdev->discov_timeout * 1000);
			queue_delayed_work(hdev->req_workqueue,
					   &hdev->discov_off, to);
		}

		err = send_settings_rsp(sk, MGMT_OP_SET_DISCOVERABLE, hdev);
>>>>>>> android-4.9
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_DISCOVERABLE, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	/* Cancel any potential discoverable timeout that might be
	 * still active and store new timeout value. The arming of
	 * the timeout happens in the complete handler.
	 */
	cancel_delayed_work(&hdev->discov_off);
	hdev->discov_timeout = timeout;

	if (cp->val)
<<<<<<< HEAD
		scan |= SCAN_INQUIRY;
=======
		hci_dev_set_flag(hdev, HCI_DISCOVERABLE);
	else
		hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
>>>>>>> android-4.9

	/* Limited discoverable mode */
	if (cp->val == 0x02)
		hci_dev_set_flag(hdev, HCI_LIMITED_DISCOVERABLE);
	else
		hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);

<<<<<<< HEAD
=======
	queue_work(hdev->req_workqueue, &hdev->discoverable_update);
	err = 0;

>>>>>>> android-4.9
failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int set_connectable(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct mgmt_mode *cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	u8 scan;
	int err;

	cp = (void *) data;

	BT_DBG("request for hci%u", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_CONNECTABLE, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_CONNECTABLE, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_CONNECTABLE, ENETDOWN);
		goto failed;
	}

	if (mgmt_pending_find(MGMT_OP_SET_DISCOVERABLE, index) ||
			mgmt_pending_find(MGMT_OP_SET_CONNECTABLE, index)) {
		err = cmd_status(sk, index, MGMT_OP_SET_CONNECTABLE, EBUSY);
		goto failed;
	}

	if (cp->val == test_bit(HCI_PSCAN, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_SET_CONNECTABLE, EALREADY);
=======
void mgmt_set_connectable_complete(struct hci_dev *hdev, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status 0x%02x", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_CONNECTABLE, hdev);
	if (!cmd)
		goto unlock;

	if (status) {
		u8 mgmt_err = mgmt_status(status);
		mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode, mgmt_err);
		goto remove_cmd;
	}

	send_settings_rsp(cmd->sk, MGMT_OP_SET_CONNECTABLE, hdev);
	new_settings(hdev, cmd->sk);

remove_cmd:
	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int set_connectable_update_settings(struct hci_dev *hdev,
					   struct sock *sk, u8 val)
{
	bool changed = false;
	int err;

	if (!!val != hci_dev_test_flag(hdev, HCI_CONNECTABLE))
		changed = true;

	if (val) {
		hci_dev_set_flag(hdev, HCI_CONNECTABLE);
	} else {
		hci_dev_clear_flag(hdev, HCI_CONNECTABLE);
		hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
	}

	err = send_settings_rsp(sk, MGMT_OP_SET_CONNECTABLE, hdev);
	if (err < 0)
		return err;

	if (changed) {
		hci_req_update_scan(hdev);
		hci_update_background_scan(hdev);
		return new_settings(hdev, sk);
	}

	return 0;
}

static int set_connectable(struct sock *sk, struct hci_dev *hdev, void *data,
			   u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED) &&
	    !hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_CONNECTABLE,
				       MGMT_STATUS_REJECTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_CONNECTABLE,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = set_connectable_update_settings(hdev, sk, cp->val);
		goto failed;
	}

	if (pending_find(MGMT_OP_SET_DISCOVERABLE, hdev) ||
	    pending_find(MGMT_OP_SET_CONNECTABLE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_CONNECTABLE,
				      MGMT_STATUS_BUSY);
>>>>>>> android-4.9
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_CONNECTABLE, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	if (cp->val)
		scan = SCAN_PAGE;
	else
		scan = 0;

	err = hci_send_cmd(hdev, HCI_OP_WRITE_SCAN_ENABLE, 1, &scan);
	if (err < 0)
		mgmt_pending_remove(cmd);
=======
	if (cp->val) {
		hci_dev_set_flag(hdev, HCI_CONNECTABLE);
	} else {
		if (hdev->discov_timeout > 0)
			cancel_delayed_work(&hdev->discov_off);

		hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);
		hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
		hci_dev_clear_flag(hdev, HCI_CONNECTABLE);
	}

	queue_work(hdev->req_workqueue, &hdev->connectable_update);
	err = 0;
>>>>>>> android-4.9

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int mgmt_event(u16 event, u16 index, void *data, u16 data_len,
							struct sock *skip_sk)
{
	struct sk_buff *skb;
	struct mgmt_hdr *hdr;
=======
static int set_bondable(struct sock *sk, struct hci_dev *hdev, void *data,
			u16 len)
{
	struct mgmt_mode *cp = data;
	bool changed;
	int err;
>>>>>>> android-4.9

	BT_DBG("hci%d %d", index, event);

<<<<<<< HEAD
	skb = alloc_skb(sizeof(*hdr) + data_len, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	bt_cb(skb)->channel = HCI_CHANNEL_CONTROL;

	hdr = (void *) skb_put(skb, sizeof(*hdr));
	hdr->opcode = cpu_to_le16(event);
	hdr->index = cpu_to_le16(index);
	hdr->len = cpu_to_le16(data_len);

	if (data)
		memcpy(skb_put(skb, data_len), data, data_len);

	hci_send_to_sock(NULL, skb, skip_sk);
	kfree_skb(skb);

	return 0;
=======
	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BONDABLE,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (cp->val)
		changed = !hci_dev_test_and_set_flag(hdev, HCI_BONDABLE);
	else
		changed = hci_dev_test_and_clear_flag(hdev, HCI_BONDABLE);

	err = send_settings_rsp(sk, MGMT_OP_SET_BONDABLE, hdev);
	if (err < 0)
		goto unlock;

	if (changed) {
		/* In limited privacy mode the change of bondable mode
		 * may affect the local advertising address.
		 */
		if (hdev_is_powered(hdev) &&
		    hci_dev_test_flag(hdev, HCI_ADVERTISING) &&
		    hci_dev_test_flag(hdev, HCI_DISCOVERABLE) &&
		    hci_dev_test_flag(hdev, HCI_LIMITED_PRIVACY))
			queue_work(hdev->req_workqueue,
				   &hdev->discoverable_update);

		err = new_settings(hdev, sk);
	}

unlock:
	hci_dev_unlock(hdev);
	return err;
>>>>>>> android-4.9
}

static int send_mode_rsp(struct sock *sk, u16 opcode, u16 index, u8 val)
{
<<<<<<< HEAD
	struct mgmt_mode rp;
=======
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	u8 val, status;
	int err;
>>>>>>> android-4.9

	rp.val = val;

<<<<<<< HEAD
	return cmd_complete(sk, index, opcode, &rp, sizeof(rp));
}
=======
	status = mgmt_bredr_support(hdev);
	if (status)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LINK_SECURITY,
				       status);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LINK_SECURITY,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);
>>>>>>> android-4.9

static int set_pairable(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct mgmt_mode *cp, ev;
	struct hci_dev *hdev;
	int err;

<<<<<<< HEAD
	cp = (void *) data;
=======
		if (!!cp->val != hci_dev_test_flag(hdev, HCI_LINK_SECURITY)) {
			hci_dev_change_flag(hdev, HCI_LINK_SECURITY);
			changed = true;
		}
>>>>>>> android-4.9

	BT_DBG("request for hci%u", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_PAIRABLE, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_PAIRABLE, ENODEV);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);
=======
	if (pending_find(MGMT_OP_SET_LINK_SECURITY, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LINK_SECURITY,
				      MGMT_STATUS_BUSY);
		goto failed;
	}
>>>>>>> android-4.9

	if (cp->val)
		set_bit(HCI_PAIRABLE, &hdev->flags);
	else
		clear_bit(HCI_PAIRABLE, &hdev->flags);

	err = send_mode_rsp(sk, MGMT_OP_SET_PAIRABLE, index, cp->val);
	if (err < 0)
		goto failed;

	ev.val = cp->val;

	err = mgmt_event(MGMT_EV_PAIRABLE, index, &ev, sizeof(ev), sk);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
#define EIR_FLAGS		0x01 /* flags */
#define EIR_UUID16_SOME		0x02 /* 16-bit UUID, more available */
#define EIR_UUID16_ALL		0x03 /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME		0x04 /* 32-bit UUID, more available */
#define EIR_UUID32_ALL		0x05 /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME	0x06 /* 128-bit UUID, more available */
#define EIR_UUID128_ALL		0x07 /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT		0x08 /* shortened local name */
#define EIR_NAME_COMPLETE	0x09 /* complete local name */
#define EIR_TX_POWER		0x0A /* transmit power level */
#define EIR_DEVICE_ID		0x10 /* device ID */
=======
static int set_ssp(struct sock *sk, struct hci_dev *hdev, void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	u8 status;
	int err;
>>>>>>> android-4.9

#define PNP_INFO_SVCLASS_ID		0x1200

<<<<<<< HEAD
static u8 bluetooth_base_uuid[] = {
			0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
			0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static u16 get_uuid16(u8 *uuid128)
{
	u32 val;
	int i;

	for (i = 0; i < 12; i++) {
		if (bluetooth_base_uuid[i] != uuid128[i])
			return 0;
	}

	memcpy(&val, &uuid128[12], 4);

	val = le32_to_cpu(val);
	if (val > 0xffff)
		return 0;
=======
	status = mgmt_bredr_support(hdev);
	if (status)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SSP, status);

	if (!lmp_ssp_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SSP,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SSP,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		bool changed;

		if (cp->val) {
			changed = !hci_dev_test_and_set_flag(hdev,
							     HCI_SSP_ENABLED);
		} else {
			changed = hci_dev_test_and_clear_flag(hdev,
							      HCI_SSP_ENABLED);
			if (!changed)
				changed = hci_dev_test_and_clear_flag(hdev,
								      HCI_HS_ENABLED);
			else
				hci_dev_clear_flag(hdev, HCI_HS_ENABLED);
		}
>>>>>>> android-4.9

	return (u16) val;
}

static void create_eir(struct hci_dev *hdev, u8 *data)
{
	u8 *ptr = data;
	u16 eir_len = 0;
	u16 uuid16_list[HCI_MAX_EIR_LENGTH / sizeof(u16)];
	int i, truncated = 0;
	struct list_head *p;
	size_t name_len;

	name_len = strnlen(hdev->dev_name, HCI_MAX_EIR_LENGTH);

<<<<<<< HEAD
	if (name_len > 0) {
		/* EIR Data type */
		if (name_len > 48) {
			name_len = 48;
			ptr[1] = EIR_NAME_SHORT;
		} else
			ptr[1] = EIR_NAME_COMPLETE;

		/* EIR Data length */
		ptr[0] = name_len + 1;
=======
	if (pending_find(MGMT_OP_SET_SSP, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SSP,
				      MGMT_STATUS_BUSY);
		goto failed;
	}

	if (!!cp->val == hci_dev_test_flag(hdev, HCI_SSP_ENABLED)) {
		err = send_settings_rsp(sk, MGMT_OP_SET_SSP, hdev);
		goto failed;
	}
>>>>>>> android-4.9

		memcpy(ptr + 2, hdev->dev_name, name_len);

<<<<<<< HEAD
		eir_len += (name_len + 2);
		ptr += (name_len + 2);
	}

	memset(uuid16_list, 0, sizeof(uuid16_list));

	/* Group all UUID16 types */
	list_for_each(p, &hdev->uuids) {
		struct bt_uuid *uuid = list_entry(p, struct bt_uuid, list);
		u16 uuid16;

		uuid16 = get_uuid16(uuid->uuid);
		if (uuid16 == 0)
			return;

		if (uuid16 < 0x1100)
			continue;

		if (uuid16 == PNP_INFO_SVCLASS_ID)
			continue;
=======
	if (!cp->val && hci_dev_test_flag(hdev, HCI_USE_DEBUG_KEYS))
		hci_send_cmd(hdev, HCI_OP_WRITE_SSP_DEBUG_MODE,
			     sizeof(cp->val), &cp->val);

	err = hci_send_cmd(hdev, HCI_OP_WRITE_SSP_MODE, 1, &cp->val);
	if (err < 0) {
		mgmt_pending_remove(cmd);
		goto failed;
	}

failed:
	hci_dev_unlock(hdev);
	return err;
}

static int set_hs(struct sock *sk, struct hci_dev *hdev, void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	bool changed;
	u8 status;
	int err;

	BT_DBG("request for %s", hdev->name);

	status = mgmt_bredr_support(hdev);
	if (status)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS, status);

	if (!lmp_ssp_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (!hci_dev_test_flag(hdev, HCI_SSP_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS,
				       MGMT_STATUS_REJECTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (pending_find(MGMT_OP_SET_SSP, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if (cp->val) {
		changed = !hci_dev_test_and_set_flag(hdev, HCI_HS_ENABLED);
	} else {
		if (hdev_is_powered(hdev)) {
			err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_HS,
					      MGMT_STATUS_REJECTED);
			goto unlock;
		}

		changed = hci_dev_test_and_clear_flag(hdev, HCI_HS_ENABLED);
	}

	err = send_settings_rsp(sk, MGMT_OP_SET_HS, hdev);
	if (err < 0)
		goto unlock;

	if (changed)
		err = new_settings(hdev, sk);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static void le_enable_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct cmd_lookup match = { NULL, hdev };

	hci_dev_lock(hdev);

	if (status) {
		u8 mgmt_err = mgmt_status(status);

		mgmt_pending_foreach(MGMT_OP_SET_LE, hdev, cmd_status_rsp,
				     &mgmt_err);
		goto unlock;
	}

	mgmt_pending_foreach(MGMT_OP_SET_LE, hdev, settings_rsp, &match);

	new_settings(hdev, match.sk);

	if (match.sk)
		sock_put(match.sk);

	/* Make sure the controller has a good default for
	 * advertising data. Restrict the update to when LE
	 * has actually been enabled. During power on, the
	 * update in powered_update_hci will take care of it.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		struct hci_request req;

		hci_req_init(&req, hdev);
		__hci_req_update_adv_data(&req, 0x00);
		__hci_req_update_scan_rsp_data(&req, 0x00);
		hci_req_run(&req, NULL);
		hci_update_background_scan(hdev);
	}

unlock:
	hci_dev_unlock(hdev);
}

static int set_le(struct sock *sk, struct hci_dev *hdev, void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	struct hci_cp_write_le_host_supported hci_cp;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;
	u8 val, enabled;
>>>>>>> android-4.9

		/* Stop if not enough space to put next UUID */
		if (eir_len + 2 + sizeof(u16) > HCI_MAX_EIR_LENGTH) {
			truncated = 1;
			break;
		}

<<<<<<< HEAD
		/* Check for duplicates */
		for (i = 0; uuid16_list[i] != 0; i++)
			if (uuid16_list[i] == uuid16)
				break;

		if (uuid16_list[i] == 0) {
			uuid16_list[i] = uuid16;
			eir_len += sizeof(u16);
		}
	}

	if (uuid16_list[0] != 0) {
		u8 *length = ptr;

		/* EIR Data type */
		ptr[1] = truncated ? EIR_UUID16_SOME : EIR_UUID16_ALL;
=======
	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LE,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LE,
				       MGMT_STATUS_INVALID_PARAMS);

	/* Bluetooth single mode LE only controllers or dual-mode
	 * controllers configured as LE only devices, do not allow
	 * switching LE off. These have either LE enabled explicitly
	 * or BR/EDR has been previously switched off.
	 *
	 * When trying to enable an already enabled LE, then gracefully
	 * send a positive response. Trying to disable it however will
	 * result into rejection.
	 */
	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		if (cp->val == 0x01)
			return send_settings_rsp(sk, MGMT_OP_SET_LE, hdev);

		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LE,
				       MGMT_STATUS_REJECTED);
	}

	hci_dev_lock(hdev);

	val = !!cp->val;
	enabled = lmp_host_le_capable(hdev);

	if (!val)
		hci_req_clear_adv_instance(hdev, NULL, NULL, 0x00, true);
>>>>>>> android-4.9

		ptr += 2;
		eir_len += 2;

<<<<<<< HEAD
		for (i = 0; uuid16_list[i] != 0; i++) {
			*ptr++ = (uuid16_list[i] & 0x00ff);
			*ptr++ = (uuid16_list[i] & 0xff00) >> 8;
=======
		if (val != hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
			hci_dev_change_flag(hdev, HCI_LE_ENABLED);
			changed = true;
		}

		if (!val && hci_dev_test_flag(hdev, HCI_ADVERTISING)) {
			hci_dev_clear_flag(hdev, HCI_ADVERTISING);
			changed = true;
>>>>>>> android-4.9
		}

		/* EIR Data length */
		*length = (i * sizeof(u16)) + 1;
	}
}

static int update_eir(struct hci_dev *hdev)
{
	struct hci_cp_write_eir cp;

	if (!(hdev->features[6] & LMP_EXT_INQ))
		return 0;

<<<<<<< HEAD
	if (hdev->ssp_mode == 0)
		return 0;
=======
	if (pending_find(MGMT_OP_SET_LE, hdev) ||
	    pending_find(MGMT_OP_SET_ADVERTISING, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_LE,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}
>>>>>>> android-4.9

	if (test_bit(HCI_SERVICE_CACHE, &hdev->flags))
		return 0;

<<<<<<< HEAD
	memset(&cp, 0, sizeof(cp));

	create_eir(hdev, cp.data);

	if (memcmp(cp.data, hdev->eir, sizeof(cp.data)) == 0)
		return 0;
=======
	hci_req_init(&req, hdev);

	memset(&hci_cp, 0, sizeof(hci_cp));

	if (val) {
		hci_cp.le = val;
		hci_cp.simul = 0x00;
	} else {
		if (hci_dev_test_flag(hdev, HCI_LE_ADV))
			__hci_req_disable_advertising(&req);
	}

	hci_req_add(&req, HCI_OP_WRITE_LE_HOST_SUPPORTED, sizeof(hci_cp),
		    &hci_cp);

	err = hci_req_run(&req, le_enable_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);
>>>>>>> android-4.9

	memcpy(hdev->eir, cp.data, sizeof(cp.data));

	return hci_send_cmd(hdev, HCI_OP_WRITE_EIR, sizeof(cp), &cp);
}

<<<<<<< HEAD
static int add_uuid(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct mgmt_cp_add_uuid *cp;
	struct hci_dev *hdev;
=======
/* This is a helper function to test for pending mgmt commands that can
 * cause CoD or EIR HCI commands. We can only allow one such pending
 * mgmt command at a time since otherwise we cannot easily track what
 * the current values are, will be, and based on that calculate if a new
 * HCI command needs to be sent and if yes with what value.
 */
static bool pending_eir_or_class(struct hci_dev *hdev)
{
	struct mgmt_pending_cmd *cmd;

	list_for_each_entry(cmd, &hdev->mgmt_pending, list) {
		switch (cmd->opcode) {
		case MGMT_OP_ADD_UUID:
		case MGMT_OP_REMOVE_UUID:
		case MGMT_OP_SET_DEV_CLASS:
		case MGMT_OP_SET_POWERED:
			return true;
		}
	}

	return false;
}

static const u8 bluetooth_base_uuid[] = {
			0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
			0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static u8 get_uuid_size(const u8 *uuid)
{
	u32 val;

	if (memcmp(uuid, bluetooth_base_uuid, 12))
		return 128;

	val = get_unaligned_le32(&uuid[12]);
	if (val > 0xffff)
		return 32;

	return 16;
}

static void mgmt_class_complete(struct hci_dev *hdev, u16 mgmt_op, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	hci_dev_lock(hdev);

	cmd = pending_find(mgmt_op, hdev);
	if (!cmd)
		goto unlock;

	mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode,
			  mgmt_status(status), hdev->dev_class, 3);

	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static void add_uuid_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	BT_DBG("status 0x%02x", status);

	mgmt_class_complete(hdev, MGMT_OP_ADD_UUID, status);
}

static int add_uuid(struct sock *sk, struct hci_dev *hdev, void *data, u16 len)
{
	struct mgmt_cp_add_uuid *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
>>>>>>> android-4.9
	struct bt_uuid *uuid;
	int err;

	cp = (void *) data;

	BT_DBG("request for hci%u", index);

<<<<<<< HEAD
	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_ADD_UUID, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_ADD_UUID, ENODEV);

	hci_dev_lock_bh(hdev);
=======
	if (pending_eir_or_class(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_UUID,
				      MGMT_STATUS_BUSY);
		goto failed;
	}
>>>>>>> android-4.9

	uuid = kmalloc(sizeof(*uuid), GFP_KERNEL);
	if (!uuid) {
		err = -ENOMEM;
		goto failed;
	}

	memcpy(uuid->uuid, cp->uuid, 16);
	uuid->svc_hint = cp->svc_hint;
	uuid->size = get_uuid_size(cp->uuid);

	list_add_tail(&uuid->list, &hdev->uuids);

<<<<<<< HEAD
	if (test_bit(HCI_UP, &hdev->flags)) {

		err = update_class(hdev);
		if (err < 0)
			goto failed;

		err = update_eir(hdev);
		if (err < 0)
			goto failed;
	} else
		err = 0;
=======
	hci_req_init(&req, hdev);

	__hci_req_update_class(&req);
	__hci_req_update_eir(&req);

	err = hci_req_run(&req, add_uuid_complete);
	if (err < 0) {
		if (err != -ENODATA)
			goto failed;

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_UUID, 0,
					hdev->dev_class, 3);
		goto failed;
	}
>>>>>>> android-4.9

	err = cmd_complete(sk, index, MGMT_OP_ADD_UUID, NULL, 0);

	err = 0;

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

<<<<<<< HEAD
	return err;
}

static int remove_uuid(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct list_head *p, *n;
	struct mgmt_cp_remove_uuid *cp;
	struct hci_dev *hdev;
=======
	if (!hci_dev_test_and_set_flag(hdev, HCI_SERVICE_CACHE)) {
		queue_delayed_work(hdev->workqueue, &hdev->service_cache,
				   CACHE_TIMEOUT);
		return true;
	}

	return false;
}

static void remove_uuid_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	BT_DBG("status 0x%02x", status);

	mgmt_class_complete(hdev, MGMT_OP_REMOVE_UUID, status);
}

static int remove_uuid(struct sock *sk, struct hci_dev *hdev, void *data,
		       u16 len)
{
	struct mgmt_cp_remove_uuid *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct bt_uuid *match, *tmp;
>>>>>>> android-4.9
	u8 bt_uuid_any[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	struct hci_request req;
	int err, found;

	cp = (void *) data;

	BT_DBG("request for hci%u", index);

<<<<<<< HEAD
	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_REMOVE_UUID, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_REMOVE_UUID, ENODEV);

	hci_dev_lock_bh(hdev);
=======
	if (pending_eir_or_class(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_REMOVE_UUID,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if (memcmp(cp->uuid, bt_uuid_any, 16) == 0) {
		hci_uuids_clear(hdev);

		if (enable_service_cache(hdev)) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_UUID,
						0, hdev->dev_class, 3);
			goto unlock;
		}
>>>>>>> android-4.9

	if (memcmp(cp->uuid, bt_uuid_any, 16) == 0) {
		err = hci_uuids_clear(hdev);
		goto unlock;
	}

	found = 0;

	list_for_each_entry_safe(match, tmp, &hdev->uuids, list) {
		if (memcmp(match->uuid, cp->uuid, 16) != 0)
			continue;

		list_del(&match->list);
		kfree(match);
		found++;
	}

	if (found == 0) {
<<<<<<< HEAD
		err = cmd_status(sk, index, MGMT_OP_REMOVE_UUID, ENOENT);
		goto unlock;
	}

	if (test_bit(HCI_UP, &hdev->flags)) {
		err = update_class(hdev);
		if (err < 0)
			goto unlock;

		err = update_eir(hdev);
		if (err < 0)
			goto unlock;
	} else
		err = 0;
=======
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_REMOVE_UUID,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}

update_class:
	hci_req_init(&req, hdev);

	__hci_req_update_class(&req);
	__hci_req_update_eir(&req);

	err = hci_req_run(&req, remove_uuid_complete);
	if (err < 0) {
		if (err != -ENODATA)
			goto unlock;

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_REMOVE_UUID, 0,
					hdev->dev_class, 3);
		goto unlock;
	}
>>>>>>> android-4.9

	err = cmd_complete(sk, index, MGMT_OP_REMOVE_UUID, NULL, 0);

	err = 0;

unlock:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int set_dev_class(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_set_dev_class *cp;
=======
static void set_class_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	BT_DBG("status 0x%02x", status);

	mgmt_class_complete(hdev, MGMT_OP_SET_DEV_CLASS, status);
}

static int set_dev_class(struct sock *sk, struct hci_dev *hdev, void *data,
			 u16 len)
{
	struct mgmt_cp_set_dev_class *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
>>>>>>> android-4.9
	int err;

	cp = (void *) data;

<<<<<<< HEAD
	BT_DBG("request for hci%u", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_DEV_CLASS, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_DEV_CLASS, ENODEV);
=======
	if (!lmp_bredr_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DEV_CLASS,
				       MGMT_STATUS_NOT_SUPPORTED);

	hci_dev_lock(hdev);

	if (pending_eir_or_class(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DEV_CLASS,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if ((cp->minor & 0x03) != 0 || (cp->major & 0xe0) != 0) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DEV_CLASS,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}
>>>>>>> android-4.9

	hci_dev_lock_bh(hdev);

	hdev->major_class &= ~MGMT_MAJOR_CLASS_MASK;
	hdev->major_class |= cp->major & MGMT_MAJOR_CLASS_MASK;
	hdev->minor_class = cp->minor;

<<<<<<< HEAD
	if (test_bit(HCI_UP, &hdev->flags)) {
		err = update_class(hdev);
		if (err == 0)
			err = cmd_complete(sk, index,
		MGMT_OP_SET_DEV_CLASS, hdev->dev_class, sizeof(u8)*3);
	} else
		err = cmd_complete(sk, index, MGMT_OP_SET_DEV_CLASS, NULL, 0);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int set_service_cache(struct sock *sk, u16 index,  unsigned char *data,
									u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_set_service_cache *cp;
	int err;
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_DEV_CLASS, 0,
					hdev->dev_class, 3);
		goto unlock;
	}

	hci_req_init(&req, hdev);

	if (hci_dev_test_and_clear_flag(hdev, HCI_SERVICE_CACHE)) {
		hci_dev_unlock(hdev);
		cancel_delayed_work_sync(&hdev->service_cache);
		hci_dev_lock(hdev);
		__hci_req_update_eir(&req);
	}

	__hci_req_update_class(&req);

	err = hci_req_run(&req, set_class_complete);
	if (err < 0) {
		if (err != -ENODATA)
			goto unlock;

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_DEV_CLASS, 0,
					hdev->dev_class, 3);
		goto unlock;
	}
>>>>>>> android-4.9

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_SERVICE_CACHE, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_SERVICE_CACHE, ENODEV);

	hci_dev_lock_bh(hdev);

	BT_DBG("hci%u enable %d", index, cp->enable);

	if (cp->enable) {
		set_bit(HCI_SERVICE_CACHE, &hdev->flags);
		err = 0;
	} else {
		clear_bit(HCI_SERVICE_CACHE, &hdev->flags);
		if (test_bit(HCI_UP, &hdev->flags)) {
			err = update_class(hdev);
			if (err == 0)
				err = update_eir(hdev);
		} else
			err = 0;
	}

<<<<<<< HEAD
	if (err == 0)
		err = cmd_complete(sk, index, MGMT_OP_SET_SERVICE_CACHE, NULL,
									0);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int load_keys(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_load_keys *cp;
	u16 key_count, expected_len;
	int i, err;

	cp = (void *) data;

	if (len < sizeof(*cp))
		return -EINVAL;
=======
	err = 0;

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int load_link_keys(struct sock *sk, struct hci_dev *hdev, void *data,
			  u16 len)
{
	struct mgmt_cp_load_link_keys *cp = data;
	const u16 max_key_count = ((U16_MAX - sizeof(*cp)) /
				   sizeof(struct mgmt_link_key_info));
	u16 key_count, expected_len;
	bool changed;
	int i;
>>>>>>> android-4.9

	BT_DBG("request for %s", hdev->name);

	if (!lmp_bredr_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LINK_KEYS,
				       MGMT_STATUS_NOT_SUPPORTED);

	key_count = __le16_to_cpu(cp->key_count);
	if (key_count > max_key_count) {
		BT_ERR("load_link_keys: too big key_count value %u",
		       key_count);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LINK_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

<<<<<<< HEAD
	expected_len = sizeof(*cp) + key_count * sizeof(struct mgmt_key_info);
	if (expected_len > len) {
		BT_ERR("load_keys: expected at least %u bytes, got %u bytes",
							expected_len, len);
		return -EINVAL;
	}

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_LOAD_KEYS, ENODEV);

	BT_DBG("hci%u debug_keys %u key_count %u", index, cp->debug_keys,
								key_count);
=======
	expected_len = sizeof(*cp) + key_count *
					sizeof(struct mgmt_link_key_info);
	if (expected_len != len) {
		BT_ERR("load_link_keys: expected %u bytes, got %u bytes",
		       expected_len, len);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LINK_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	if (cp->debug_keys != 0x00 && cp->debug_keys != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LINK_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);

	BT_DBG("%s debug_keys %u key_count %u", hdev->name, cp->debug_keys,
	       key_count);

	for (i = 0; i < key_count; i++) {
		struct mgmt_link_key_info *key = &cp->keys[i];

		if (key->addr.type != BDADDR_BREDR || key->type > 0x08)
			return mgmt_cmd_status(sk, hdev->id,
					       MGMT_OP_LOAD_LINK_KEYS,
					       MGMT_STATUS_INVALID_PARAMS);
	}
>>>>>>> android-4.9

	hci_dev_lock_bh(hdev);

	hci_link_keys_clear(hdev);

<<<<<<< HEAD
	set_bit(HCI_LINK_KEYS, &hdev->flags);

	if (cp->debug_keys)
		set_bit(HCI_DEBUG_KEYS, &hdev->flags);
	else
		clear_bit(HCI_DEBUG_KEYS, &hdev->flags);

	len -= sizeof(*cp);
	i = 0;

	while (i < len) {
		struct mgmt_key_info *key = (void *) cp->keys + i;

		i += sizeof(*key);

		if (key->key_type == KEY_TYPE_LTK) {
			struct key_master_id *id = (void *) key->data;
=======
	if (cp->debug_keys)
		changed = !hci_dev_test_and_set_flag(hdev, HCI_KEEP_DEBUG_KEYS);
	else
		changed = hci_dev_test_and_clear_flag(hdev,
						      HCI_KEEP_DEBUG_KEYS);

	if (changed)
		new_settings(hdev, NULL);
>>>>>>> android-4.9

			if (key->dlen != sizeof(struct key_master_id))
				continue;

			hci_add_ltk(hdev, 0, &key->bdaddr, key->addr_type,
					key->pin_len, key->auth, id->ediv,
					id->rand, key->val);

			continue;
		}

<<<<<<< HEAD
		hci_add_link_key(hdev, 0, &key->bdaddr, key->val, key->key_type,
								key->pin_len);
	}

	err = cmd_complete(sk, index, MGMT_OP_LOAD_KEYS, NULL, 0);
=======
		/* Always ignore debug keys and require a new pairing if
		 * the user wants to use them.
		 */
		if (key->type == HCI_LK_DEBUG_COMBINATION)
			continue;

		hci_add_link_key(hdev, NULL, &key->addr.bdaddr, key->val,
				 key->type, key->pin_len, NULL);
	}

	mgmt_cmd_complete(sk, hdev->id, MGMT_OP_LOAD_LINK_KEYS, 0, NULL, 0);
>>>>>>> android-4.9

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int remove_key(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_remove_key *cp;
	struct hci_conn *conn;
	int err;

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_REMOVE_KEY, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_REMOVE_KEY, ENODEV);

	hci_dev_lock_bh(hdev);

	err = hci_remove_link_key(hdev, &cp->bdaddr);
	if (err < 0) {
		err = cmd_status(sk, index, MGMT_OP_REMOVE_KEY, -err);
		goto unlock;
	}

	err = 0;

	if (!test_bit(HCI_UP, &hdev->flags) || !cp->disconnect)
		goto unlock;

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &cp->bdaddr);
	if (conn) {
		struct hci_cp_disconnect dc;

		put_unaligned_le16(conn->handle, &dc.handle);
		dc.reason = 0x13; /* Remote User Terminated Connection */
		err = hci_send_cmd(hdev, HCI_OP_DISCONNECT, 0, NULL);
	}

unlock:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int disconnect(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
<<<<<<< HEAD
	struct hci_dev *hdev;
	struct mgmt_cp_disconnect *cp;
	struct hci_cp_disconnect dc;
	struct pending_cmd *cmd;
=======
	struct mgmt_cp_unpair_device *cp = data;
	struct mgmt_rp_unpair_device rp;
	struct hci_conn_params *params;
	struct mgmt_pending_cmd *cmd;
>>>>>>> android-4.9
	struct hci_conn *conn;
	u8 addr_type;
	int err;

<<<<<<< HEAD
	BT_DBG("");

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_DISCONNECT, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_DISCONNECT, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_DISCONNECT, ENETDOWN);
		goto failed;
	}

	if (mgmt_pending_find(MGMT_OP_DISCONNECT, index)) {
		err = cmd_status(sk, index, MGMT_OP_DISCONNECT, EBUSY);
		goto failed;
	}

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &cp->bdaddr);
	if (!conn) {
		conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->bdaddr);
		if (!conn) {
			err = cmd_status(sk, index, MGMT_OP_DISCONNECT,
							ENOTCONN);
			goto failed;
		}
=======
	memset(&rp, 0, sizeof(rp));
	bacpy(&rp.addr.bdaddr, &cp->addr.bdaddr);
	rp.addr.type = cp->addr.type;

	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNPAIR_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	if (cp->disconnect != 0x00 && cp->disconnect != 0x01)
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNPAIR_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNPAIR_DEVICE,
					MGMT_STATUS_NOT_POWERED, &rp,
					sizeof(rp));
		goto unlock;
	}

	if (cp->addr.type == BDADDR_BREDR) {
		/* If disconnection is requested, then look up the
		 * connection. If the remote device is connected, it
		 * will be later used to terminate the link.
		 *
		 * Setting it to NULL explicitly will cause no
		 * termination of the link.
		 */
		if (cp->disconnect)
			conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK,
						       &cp->addr.bdaddr);
		else
			conn = NULL;

		err = hci_remove_link_key(hdev, &cp->addr.bdaddr);
		if (err < 0) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_UNPAIR_DEVICE,
						MGMT_STATUS_NOT_PAIRED, &rp,
						sizeof(rp));
			goto unlock;
		}

		goto done;
	}

	/* LE address type */
	addr_type = le_addr_type(cp->addr.type);

	hci_remove_irk(hdev, &cp->addr.bdaddr, addr_type);

	err = hci_remove_ltk(hdev, &cp->addr.bdaddr, addr_type);
	if (err < 0) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNPAIR_DEVICE,
					MGMT_STATUS_NOT_PAIRED, &rp,
					sizeof(rp));
		goto unlock;
	}

	conn = hci_conn_hash_lookup_le(hdev, &cp->addr.bdaddr, addr_type);
	if (!conn) {
		hci_conn_params_del(hdev, &cp->addr.bdaddr, addr_type);
		goto done;
	}

	/* Abort any ongoing SMP pairing */
	smp_cancel_pairing(conn);

	/* Defer clearing up the connection parameters until closing to
	 * give a chance of keeping them if a repairing happens.
	 */
	set_bit(HCI_CONN_PARAM_REMOVAL_PEND, &conn->flags);

	/* Disable auto-connection parameters if present */
	params = hci_conn_params_lookup(hdev, &cp->addr.bdaddr, addr_type);
	if (params) {
		if (params->explicit_connect)
			params->auto_connect = HCI_AUTO_CONN_EXPLICIT;
		else
			params->auto_connect = HCI_AUTO_CONN_DISABLED;
	}

	/* If disconnection is not requested, then clear the connection
	 * variable so that the link is not terminated.
	 */
	if (!cp->disconnect)
		conn = NULL;

done:
	/* If the connection variable is set, then termination of the
	 * link is requested.
	 */
	if (!conn) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNPAIR_DEVICE, 0,
					&rp, sizeof(rp));
		device_unpaired(hdev, &cp->addr.bdaddr, cp->addr.type, sk);
		goto unlock;
>>>>>>> android-4.9
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_DISCONNECT, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	put_unaligned_le16(conn->handle, &dc.handle);
	dc.reason = 0x13; /* Remote User Terminated Connection */

	err = hci_send_cmd(hdev, HCI_OP_DISCONNECT, sizeof(dc), &dc);
=======
	cmd->cmd_complete = addr_cmd_complete;

	err = hci_abort_conn(conn, HCI_ERROR_REMOTE_USER_TERM);
>>>>>>> android-4.9
	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int get_connections(struct sock *sk, u16 index)
{
	struct mgmt_rp_get_connections *rp;
	struct hci_dev *hdev;
	struct list_head *p;
	size_t rp_len;
	u16 count;
	int i, err;

	BT_DBG("");

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_GET_CONNECTIONS, ENODEV);

	hci_dev_lock_bh(hdev);

	count = 0;
	list_for_each(p, &hdev->conn_hash.list) {
		count++;
	}

	rp_len = sizeof(*rp) + (count * sizeof(bdaddr_t));
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp) {
		err = -ENOMEM;
		goto unlock;
	}

	put_unaligned_le16(count, &rp->conn_count);

	read_lock(&hci_dev_list_lock);

	i = 0;
	list_for_each(p, &hdev->conn_hash.list) {
		struct hci_conn *c = list_entry(p, struct hci_conn, list);

		bacpy(&rp->conn[i++], &c->dst);
	}

	read_unlock(&hci_dev_list_lock);

	err = cmd_complete(sk, index, MGMT_OP_GET_CONNECTIONS, rp, rp_len);

unlock:
	kfree(rp);
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
	return err;
}

static int pin_code_reply(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
<<<<<<< HEAD
	struct hci_dev *hdev;
	struct mgmt_cp_pin_code_reply *cp;
	struct hci_cp_pin_code_reply reply;
	struct pending_cmd *cmd;
=======
	struct mgmt_cp_disconnect *cp = data;
	struct mgmt_rp_disconnect rp;
	struct mgmt_pending_cmd *cmd;
	struct hci_conn *conn;
>>>>>>> android-4.9
	int err;

	BT_DBG("");

<<<<<<< HEAD
	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_PIN_CODE_REPLY, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_PIN_CODE_REPLY, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_PIN_CODE_REPLY, ENETDOWN);
=======
	memset(&rp, 0, sizeof(rp));
	bacpy(&rp.addr.bdaddr, &cp->addr.bdaddr);
	rp.addr.type = cp->addr.type;

	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_DISCONNECT,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	hci_dev_lock(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_DISCONNECT,
					MGMT_STATUS_NOT_POWERED, &rp,
					sizeof(rp));
		goto failed;
	}

	if (pending_find(MGMT_OP_DISCONNECT, hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_DISCONNECT,
					MGMT_STATUS_BUSY, &rp, sizeof(rp));
		goto failed;
	}

	if (cp->addr.type == BDADDR_BREDR)
		conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK,
					       &cp->addr.bdaddr);
	else
		conn = hci_conn_hash_lookup_le(hdev, &cp->addr.bdaddr,
					       le_addr_type(cp->addr.type));

	if (!conn || conn->state == BT_OPEN || conn->state == BT_CLOSED) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_DISCONNECT,
					MGMT_STATUS_NOT_CONNECTED, &rp,
					sizeof(rp));
>>>>>>> android-4.9
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_PIN_CODE_REPLY, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	bacpy(&reply.bdaddr, &cp->bdaddr);
	reply.pin_len = cp->pin_len;
	memcpy(reply.pin_code, cp->pin_code, 16);

	err = hci_send_cmd(hdev, HCI_OP_PIN_CODE_REPLY, sizeof(reply), &reply);
=======
	cmd->cmd_complete = generic_cmd_complete;

	err = hci_disconnect(conn, HCI_ERROR_REMOTE_USER_TERM);
>>>>>>> android-4.9
	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int encrypt_link(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_encrypt_link *cp;
	struct hci_cp_set_conn_encrypt enc;
	struct hci_conn *conn;
	int err = 0;

	BT_DBG("");

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_ENCRYPT_LINK, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_ENCRYPT_LINK, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_ENCRYPT_LINK, ENETDOWN);
		goto done;
	}

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &cp->bdaddr);
	if (!conn) {
		err = cmd_status(sk, index, MGMT_OP_ENCRYPT_LINK, ENOTCONN);
		goto done;
	}

	if (test_and_set_bit(HCI_CONN_ENCRYPT_PEND, &conn->pend)) {
		err = cmd_status(sk, index, MGMT_OP_ENCRYPT_LINK, EINPROGRESS);
		goto done;
	}

	if (conn->link_mode & HCI_LM_AUTH) {
		enc.handle = cpu_to_le16(conn->handle);
		enc.encrypt = cp->enable;
		err = hci_send_cmd(hdev,
				HCI_OP_SET_CONN_ENCRYPT, sizeof(enc), &enc);
	} else {
		conn->auth_initiator = 1;
		if (!test_and_set_bit(HCI_CONN_AUTH_PEND, &conn->pend)) {
			struct hci_cp_auth_requested cp;
			cp.handle = cpu_to_le16(conn->handle);
			err = hci_send_cmd(conn->hdev,
				HCI_OP_AUTH_REQUESTED, sizeof(cp), &cp);
		}
=======
static u8 link_to_bdaddr(u8 link_type, u8 addr_type)
{
	switch (link_type) {
	case LE_LINK:
		switch (addr_type) {
		case ADDR_LE_DEV_PUBLIC:
			return BDADDR_LE_PUBLIC;

		default:
			/* Fallback to LE Random address type */
			return BDADDR_LE_RANDOM;
		}

	default:
		/* Fallback to BR/EDR type */
		return BDADDR_BREDR;
>>>>>>> android-4.9
	}

done:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}


static int pin_code_neg_reply(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_pin_code_neg_reply *cp;
	struct pending_cmd *cmd;
	int err;

	BT_DBG("");

	cp = (void *) data;

<<<<<<< HEAD
	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_PIN_CODE_NEG_REPLY,
									EINVAL);
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_GET_CONNECTIONS,
				      MGMT_STATUS_NOT_POWERED);
		goto unlock;
	}
>>>>>>> android-4.9

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_PIN_CODE_NEG_REPLY,
									ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_PIN_CODE_NEG_REPLY,
								ENETDOWN);
		goto failed;
	}

<<<<<<< HEAD
	cmd = mgmt_pending_add(sk, MGMT_OP_PIN_CODE_NEG_REPLY, index,
								data, len);
	if (!cmd) {
=======
	rp_len = sizeof(*rp) + (i * sizeof(struct mgmt_addr_info));
	rp = kmalloc(rp_len, GFP_KERNEL);
	if (!rp) {
>>>>>>> android-4.9
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	err = hci_send_cmd(hdev, HCI_OP_PIN_CODE_NEG_REPLY, sizeof(cp->bdaddr),
								&cp->bdaddr);
	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
=======
	i = 0;
	list_for_each_entry(c, &hdev->conn_hash.list, list) {
		if (!test_bit(HCI_CONN_MGMT_CONNECTED, &c->flags))
			continue;
		bacpy(&rp->addr[i].bdaddr, &c->dst);
		rp->addr[i].type = link_to_bdaddr(c->type, c->dst_type);
		if (c->type == SCO_LINK || c->type == ESCO_LINK)
			continue;
		i++;
	}

	rp->conn_count = cpu_to_le16(i);
>>>>>>> android-4.9

	return err;
}

<<<<<<< HEAD
static int le_add_dev_white_list(struct sock *sk, u16 index,
					unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_le_add_dev_white_list *cp;
	int err = 0;
=======
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONNECTIONS, 0, rp,
				rp_len);
>>>>>>> android-4.9

	BT_DBG("");

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_LE_ADD_DEV_WHITE_LIST,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_LE_ADD_DEV_WHITE_LIST,
									ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_LE_ADD_DEV_WHITE_LIST,
								ENETDOWN);
		goto failed;
	}

	hci_le_add_dev_white_list(hdev, &cp->bdaddr);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int le_remove_dev_white_list(struct sock *sk, u16 index,
					unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_le_remove_dev_white_list *cp;
	int err = 0;

	BT_DBG("");

	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_LE_REMOVE_DEV_WHITE_LIST,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_LE_REMOVE_DEV_WHITE_LIST,
									ENODEV);

	hci_dev_lock_bh(hdev);
=======
static int send_pin_code_neg_reply(struct sock *sk, struct hci_dev *hdev,
				   struct mgmt_cp_pin_code_neg_reply *cp)
{
	struct mgmt_pending_cmd *cmd;
	int err;
>>>>>>> android-4.9

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_LE_REMOVE_DEV_WHITE_LIST,
								ENETDOWN);
		goto failed;
	}

<<<<<<< HEAD
	hci_le_remove_dev_white_list(hdev, &cp->bdaddr);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
=======
	cmd->cmd_complete = addr_cmd_complete;

	err = hci_send_cmd(hdev, HCI_OP_PIN_CODE_NEG_REPLY,
			   sizeof(cp->addr.bdaddr), &cp->addr.bdaddr);
	if (err < 0)
		mgmt_pending_remove(cmd);
>>>>>>> android-4.9

	return err;
}

static int le_create_conn_white_list(struct sock *sk, u16 index)
{
	struct hci_dev *hdev;
	struct hci_conn *conn;
<<<<<<< HEAD
	u8 sec_level, auth_type;
	struct pending_cmd *cmd;
	bdaddr_t bdaddr;
	int err = 0;
=======
	struct mgmt_cp_pin_code_reply *cp = data;
	struct hci_cp_pin_code_reply reply;
	struct mgmt_pending_cmd *cmd;
	int err;
>>>>>>> android-4.9

	BT_DBG("");

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_LE_CREATE_CONN_WHITE_LIST,
									ENODEV);

	hci_dev_lock_bh(hdev);

<<<<<<< HEAD
	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_LE_CREATE_CONN_WHITE_LIST,
								ENETDOWN);
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_LE_CREATE_CONN_WHITE_LIST, index,
								NULL, 0);
	if (!cmd) {
		err = -ENOMEM;
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_PIN_CODE_REPLY,
				      MGMT_STATUS_NOT_POWERED);
		goto failed;
	}

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &cp->addr.bdaddr);
	if (!conn) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_PIN_CODE_REPLY,
				      MGMT_STATUS_NOT_CONNECTED);
>>>>>>> android-4.9
		goto failed;
	}

	sec_level = BT_SECURITY_MEDIUM;
	auth_type = HCI_AT_GENERAL_BONDING;
	memset(&bdaddr, 0, sizeof(bdaddr));
	conn = hci_le_connect(hdev, 0, BDADDR_ANY, sec_level, auth_type, NULL);
	if (IS_ERR(conn)) {
		err = PTR_ERR(conn);
		mgmt_pending_remove(cmd);
	}

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int le_cancel_create_conn_white_list(struct sock *sk, u16 index)
{
	struct hci_dev *hdev;
	int err = 0;

	BT_DBG("");

<<<<<<< HEAD
	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index,
			MGMT_OP_LE_CANCEL_CREATE_CONN_WHITE_LIST, ENODEV);
=======
		err = send_pin_code_neg_reply(sk, hdev, &ncp);
		if (err >= 0)
			err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_PIN_CODE_REPLY,
					      MGMT_STATUS_INVALID_PARAMS);
>>>>>>> android-4.9

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index,
			MGMT_OP_LE_CANCEL_CREATE_CONN_WHITE_LIST, ENETDOWN);
		goto failed;
	}

<<<<<<< HEAD
	hci_le_cancel_create_connect(hdev, BDADDR_ANY);
=======
	cmd->cmd_complete = addr_cmd_complete;

	bacpy(&reply.bdaddr, &cp->addr.bdaddr);
	reply.pin_len = cp->pin_len;
	memcpy(reply.pin_code, cp->pin_code, sizeof(reply.pin_code));

	err = hci_send_cmd(hdev, HCI_OP_PIN_CODE_REPLY, sizeof(reply), &reply);
	if (err < 0)
		mgmt_pending_remove(cmd);
>>>>>>> android-4.9

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int le_clear_white_list(struct sock *sk, u16 index)
{
	struct hci_dev *hdev;
	int err;

	BT_DBG("");

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index,
			MGMT_OP_LE_CLEAR_WHITE_LIST, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index,
			MGMT_OP_LE_CLEAR_WHITE_LIST, ENETDOWN);
		goto failed;
	}

	err = hci_send_cmd(hdev, HCI_OP_LE_CLEAR_WHITE_LIST, 0, NULL);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int set_io_capability(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
=======
static int set_io_capability(struct sock *sk, struct hci_dev *hdev, void *data,
			     u16 len)
>>>>>>> android-4.9
{
	struct hci_dev *hdev;
	struct mgmt_cp_set_io_capability *cp;

	BT_DBG("");

<<<<<<< HEAD
	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_IO_CAPABILITY, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_IO_CAPABILITY, ENODEV);

	hci_dev_lock_bh(hdev);
=======
	if (cp->io_capability > SMP_IO_KEYBOARD_DISPLAY)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_IO_CAPABILITY,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);
>>>>>>> android-4.9

	hdev->io_capability = cp->io_capability;

	BT_DBG("%s IO capability set to 0x%02x", hdev->name,
	       hdev->io_capability);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

<<<<<<< HEAD
	return cmd_complete(sk, index, MGMT_OP_SET_IO_CAPABILITY, NULL, 0);
=======
	return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_IO_CAPABILITY, 0,
				 NULL, 0);
>>>>>>> android-4.9
}

static struct mgmt_pending_cmd *find_pairing(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
<<<<<<< HEAD
	struct list_head *p;

	list_for_each(p, &cmd_list) {
		struct pending_cmd *cmd;

		cmd = list_entry(p, struct pending_cmd, list);
=======
	struct mgmt_pending_cmd *cmd;
>>>>>>> android-4.9

		if (cmd->opcode != MGMT_OP_PAIR_DEVICE)
			continue;

		if (cmd->index != hdev->id)
			continue;

		if (cmd->user_data != conn)
			continue;

		return cmd;
	}

	return NULL;
}

static int pairing_complete(struct mgmt_pending_cmd *cmd, u8 status)
{
	struct mgmt_rp_pair_device rp;
	struct hci_conn *conn = cmd->user_data;
	int err;

<<<<<<< HEAD
	BT_DBG(" %u", status);

	bacpy(&rp.bdaddr, &conn->dst);
	rp.status = status;

	cmd_complete(cmd->sk, cmd->index, MGMT_OP_PAIR_DEVICE, &rp, sizeof(rp));
=======
	bacpy(&rp.addr.bdaddr, &conn->dst);
	rp.addr.type = link_to_bdaddr(conn->type, conn->dst_type);

	err = mgmt_cmd_complete(cmd->sk, cmd->index, MGMT_OP_PAIR_DEVICE,
				status, &rp, sizeof(rp));
>>>>>>> android-4.9

	/* So we don't get further callbacks for this connection */
	conn->connect_cfm_cb = NULL;
	conn->security_cfm_cb = NULL;
	conn->disconn_cfm_cb = NULL;

<<<<<<< HEAD
	mgmt_pending_remove(cmd);
=======
	hci_conn_drop(conn);

	/* The device is paired so there is no need to remove
	 * its connection parameters anymore.
	 */
	clear_bit(HCI_CONN_PARAM_REMOVAL_PEND, &conn->flags);

	hci_conn_put(conn);

	return err;
}

void mgmt_smp_complete(struct hci_conn *conn, bool complete)
{
	u8 status = complete ? MGMT_STATUS_SUCCESS : MGMT_STATUS_FAILED;
	struct mgmt_pending_cmd *cmd;

	cmd = find_pairing(conn);
	if (cmd) {
		cmd->cmd_complete(cmd, status);
		mgmt_pending_remove(cmd);
	}
>>>>>>> android-4.9
}

static void pairing_complete_cb(struct hci_conn *conn, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG(" %u", status);

	cmd = find_pairing(conn);
	if (!cmd) {
<<<<<<< HEAD
		BT_DBG("Unable to find a pending command");
		return;
	}

	pairing_complete(cmd, status);
	hci_conn_put(conn);
}

static void pairing_security_complete_cb(struct hci_conn *conn, u8 status)
{
	struct pending_cmd *cmd;

	BT_DBG(" %u", status);

	cmd = find_pairing(conn);
	if (!cmd) {
=======
>>>>>>> android-4.9
		BT_DBG("Unable to find a pending command");
		return;
	}

<<<<<<< HEAD
	if (conn->type == LE_LINK)
		smp_link_encrypt_cmplt(conn->l2cap_data, status,
				status ? 0 : 1);
	else
		pairing_complete(cmd, status);
}

static void pairing_connect_complete_cb(struct hci_conn *conn, u8 status)
{
	struct pending_cmd *cmd;

	BT_DBG("conn: %p %u", conn, status);
=======
	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);
}

static void le_pairing_complete_cb(struct hci_conn *conn, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status %u", status);

	if (!status)
		return;
>>>>>>> android-4.9

	cmd = find_pairing(conn);
	if (!cmd) {
		BT_DBG("Unable to find a pending command");
		return;
	}

<<<<<<< HEAD
	if (status || conn->pending_sec_level < BT_SECURITY_MEDIUM)
		pairing_complete(cmd, status);

	hci_conn_put(conn);
}

static void discovery_terminated(struct pending_cmd *cmd, void *data)
{
	struct hci_dev *hdev;
	struct mgmt_mode ev = {0};

	BT_DBG("");
	hdev = hci_dev_get(cmd->index);
	if (!hdev)
		goto not_found;

	del_timer(&hdev->disco_le_timer);
	del_timer(&hdev->disco_timer);
	hci_dev_put(hdev);

not_found:
	mgmt_event(MGMT_EV_DISCOVERING, cmd->index, &ev, sizeof(ev), NULL);

	list_del(&cmd->list);

	mgmt_pending_free(cmd);
}

static int pair_device(struct sock *sk, u16 index, unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_pair_device *cp;
	struct pending_cmd *cmd;
	u8 sec_level, auth_type, io_cap;
	struct hci_conn *conn;
	struct adv_entry *entry;
=======
	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);
}

static int pair_device(struct sock *sk, struct hci_dev *hdev, void *data,
		       u16 len)
{
	struct mgmt_cp_pair_device *cp = data;
	struct mgmt_rp_pair_device rp;
	struct mgmt_pending_cmd *cmd;
	u8 sec_level, auth_type;
	struct hci_conn *conn;
>>>>>>> android-4.9
	int err;

	BT_DBG("");

<<<<<<< HEAD
	cp = (void *) data;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_PAIR_DEVICE, EINVAL);

	hdev = hci_dev_get(index);

	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_PAIR_DEVICE, ENODEV);

	hci_dev_lock_bh(hdev);

	io_cap = cp->io_cap;

	sec_level = BT_SECURITY_MEDIUM;
	auth_type = HCI_AT_DEDICATED_BONDING;

	entry = hci_find_adv_entry(hdev, &cp->bdaddr);
	if (entry && entry->flags & 0x04) {
		conn = hci_le_connect(hdev, 0, &cp->bdaddr, sec_level,
							auth_type, NULL);
	} else {
		/* ACL-SSP does not support io_cap 0x04 (KeyboadDisplay) */
		if (io_cap == 0x04)
			io_cap = 0x01;
		conn = hci_connect(hdev, ACL_LINK, 0, &cp->bdaddr, sec_level,
								auth_type);
		conn->auth_initiator = 1;
	}

	if (IS_ERR(conn)) {
		err = PTR_ERR(conn);
=======
	memset(&rp, 0, sizeof(rp));
	bacpy(&rp.addr.bdaddr, &cp->addr.bdaddr);
	rp.addr.type = cp->addr.type;

	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	if (cp->io_cap > SMP_IO_KEYBOARD_DISPLAY)
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					MGMT_STATUS_NOT_POWERED, &rp,
					sizeof(rp));
		goto unlock;
	}

	if (hci_bdaddr_is_paired(hdev, &cp->addr.bdaddr, cp->addr.type)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					MGMT_STATUS_ALREADY_PAIRED, &rp,
					sizeof(rp));
		goto unlock;
	}

	sec_level = BT_SECURITY_MEDIUM;
	auth_type = HCI_AT_DEDICATED_BONDING;

	if (cp->addr.type == BDADDR_BREDR) {
		conn = hci_connect_acl(hdev, &cp->addr.bdaddr, sec_level,
				       auth_type);
	} else {
		u8 addr_type = le_addr_type(cp->addr.type);
		struct hci_conn_params *p;

		/* When pairing a new device, it is expected to remember
		 * this device for future connections. Adding the connection
		 * parameter information ahead of time allows tracking
		 * of the slave preferred values and will speed up any
		 * further connection establishment.
		 *
		 * If connection parameters already exist, then they
		 * will be kept and this function does nothing.
		 */
		p = hci_conn_params_add(hdev, &cp->addr.bdaddr, addr_type);

		if (p->auto_connect == HCI_AUTO_CONN_EXPLICIT)
			p->auto_connect = HCI_AUTO_CONN_DISABLED;

		conn = hci_connect_le_scan(hdev, &cp->addr.bdaddr,
					   addr_type, sec_level,
					   HCI_LE_CONN_TIMEOUT);
	}

	if (IS_ERR(conn)) {
		int status;

		if (PTR_ERR(conn) == -EBUSY)
			status = MGMT_STATUS_BUSY;
		else if (PTR_ERR(conn) == -EOPNOTSUPP)
			status = MGMT_STATUS_NOT_SUPPORTED;
		else if (PTR_ERR(conn) == -ECONNREFUSED)
			status = MGMT_STATUS_REJECTED;
		else
			status = MGMT_STATUS_CONNECT_FAILED;

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					status, &rp, sizeof(rp));
>>>>>>> android-4.9
		goto unlock;
	}

	if (conn->connect_cfm_cb) {
<<<<<<< HEAD
		hci_conn_put(conn);
		err = cmd_status(sk, index, MGMT_OP_PAIR_DEVICE, EBUSY);
=======
		hci_conn_drop(conn);
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_PAIR_DEVICE,
					MGMT_STATUS_BUSY, &rp, sizeof(rp));
>>>>>>> android-4.9
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_PAIR_DEVICE, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		hci_conn_drop(conn);
		goto unlock;
	}

<<<<<<< HEAD
	conn->connect_cfm_cb = pairing_connect_complete_cb;
	conn->security_cfm_cb = pairing_security_complete_cb;
	conn->disconn_cfm_cb = pairing_complete_cb;
	conn->io_capability = io_cap;
	cmd->user_data = conn;
=======
	cmd->cmd_complete = pairing_complete;

	/* For LE, just connecting isn't a proof that the pairing finished */
	if (cp->addr.type == BDADDR_BREDR) {
		conn->connect_cfm_cb = pairing_complete_cb;
		conn->security_cfm_cb = pairing_complete_cb;
		conn->disconn_cfm_cb = pairing_complete_cb;
	} else {
		conn->connect_cfm_cb = le_pairing_complete_cb;
		conn->security_cfm_cb = le_pairing_complete_cb;
		conn->disconn_cfm_cb = le_pairing_complete_cb;
	}

	conn->io_capability = cp->io_cap;
	cmd->user_data = hci_conn_get(conn);
>>>>>>> android-4.9

	if ((conn->state == BT_CONNECTED || conn->state == BT_CONFIG) &&
	    hci_conn_security(conn, sec_level, auth_type, true)) {
		cmd->cmd_complete(cmd, 0);
		mgmt_pending_remove(cmd);
	}

	err = 0;

unlock:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int user_confirm_reply(struct sock *sk, u16 index, unsigned char *data,
							u16 len, u16 opcode)
{
<<<<<<< HEAD
	struct mgmt_cp_user_confirm_reply *cp = (void *) data;
	u16 mgmt_op = opcode, hci_op;
	struct pending_cmd *cmd;
	struct hci_dev *hdev;
	struct hci_conn *le_conn;
=======
	struct mgmt_addr_info *addr = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_conn *conn;
>>>>>>> android-4.9
	int err;

	BT_DBG("%d", mgmt_op);

	if (mgmt_op == MGMT_OP_USER_CONFIRM_NEG_REPLY)
		hci_op = HCI_OP_USER_CONFIRM_NEG_REPLY;
	else
		hci_op = HCI_OP_USER_CONFIRM_REPLY;

<<<<<<< HEAD
	if (len < sizeof(*cp))
		return cmd_status(sk, index, mgmt_op, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, mgmt_op, ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, mgmt_op, ENETDOWN);
		goto done;
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_CANCEL_PAIR_DEVICE,
				      MGMT_STATUS_NOT_POWERED);
		goto unlock;
	}

	cmd = pending_find(MGMT_OP_PAIR_DEVICE, hdev);
	if (!cmd) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_CANCEL_PAIR_DEVICE,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
>>>>>>> android-4.9
	}

	le_conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->bdaddr);
	if (le_conn) {
		err = le_user_confirm_reply(le_conn, mgmt_op, (void *) cp);
		goto done;
	}
	BT_DBG("BR/EDR: %s", mgmt_op == MGMT_OP_USER_CONFIRM_NEG_REPLY ?
							"Reject" : "Accept");

<<<<<<< HEAD
	cmd = mgmt_pending_add(sk, mgmt_op, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto done;
	}

	err = hci_send_cmd(hdev, hci_op, sizeof(cp->bdaddr), &cp->bdaddr);
	if (err < 0)
		mgmt_pending_remove(cmd);

done:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int resolve_name(struct sock *sk, u16 index, unsigned char *data,
								u16 len)
{
	struct mgmt_cp_resolve_name *mgmt_cp = (void *) data;
	struct hci_cp_remote_name_req hci_cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	int err;

	BT_DBG("");

	if (len != sizeof(*mgmt_cp))
		return cmd_status(sk, index, MGMT_OP_RESOLVE_NAME, EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_RESOLVE_NAME, ENODEV);
=======
	if (bacmp(&addr->bdaddr, &conn->dst) != 0) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_CANCEL_PAIR_DEVICE,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}

	cmd->cmd_complete(cmd, MGMT_STATUS_CANCELLED);
	mgmt_pending_remove(cmd);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_CANCEL_PAIR_DEVICE, 0,
				addr, sizeof(*addr));
unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int user_pairing_resp(struct sock *sk, struct hci_dev *hdev,
			     struct mgmt_addr_info *addr, u16 mgmt_op,
			     u16 hci_op, __le32 passkey)
{
	struct mgmt_pending_cmd *cmd;
	struct hci_conn *conn;
	int err;

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, mgmt_op,
					MGMT_STATUS_NOT_POWERED, addr,
					sizeof(*addr));
		goto done;
	}

	if (addr->type == BDADDR_BREDR)
		conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &addr->bdaddr);
	else
		conn = hci_conn_hash_lookup_le(hdev, &addr->bdaddr,
					       le_addr_type(addr->type));

	if (!conn) {
		err = mgmt_cmd_complete(sk, hdev->id, mgmt_op,
					MGMT_STATUS_NOT_CONNECTED, addr,
					sizeof(*addr));
		goto done;
	}

	if (addr->type == BDADDR_LE_PUBLIC || addr->type == BDADDR_LE_RANDOM) {
		err = smp_user_confirm_reply(conn, mgmt_op, passkey);
		if (!err)
			err = mgmt_cmd_complete(sk, hdev->id, mgmt_op,
						MGMT_STATUS_SUCCESS, addr,
						sizeof(*addr));
		else
			err = mgmt_cmd_complete(sk, hdev->id, mgmt_op,
						MGMT_STATUS_FAILED, addr,
						sizeof(*addr));
>>>>>>> android-4.9

	hci_dev_lock_bh(hdev);

<<<<<<< HEAD
	cmd = mgmt_pending_add(sk, MGMT_OP_RESOLVE_NAME, index, data, len);
=======
	cmd = mgmt_pending_add(sk, mgmt_op, hdev, addr, sizeof(*addr));
>>>>>>> android-4.9
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	memset(&hci_cp, 0, sizeof(hci_cp));
	bacpy(&hci_cp.bdaddr, &mgmt_cp->bdaddr);
	err = hci_send_cmd(hdev, HCI_OP_REMOTE_NAME_REQ, sizeof(hci_cp),
								&hci_cp);
=======
	cmd->cmd_complete = addr_cmd_complete;

	/* Continue with pairing via HCI */
	if (hci_op == HCI_OP_USER_PASSKEY_REPLY) {
		struct hci_cp_user_passkey_reply cp;

		bacpy(&cp.bdaddr, &addr->bdaddr);
		cp.passkey = passkey;
		err = hci_send_cmd(hdev, hci_op, sizeof(cp), &cp);
	} else
		err = hci_send_cmd(hdev, hci_op, sizeof(addr->bdaddr),
				   &addr->bdaddr);

>>>>>>> android-4.9
	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static int cancel_resolve_name(struct sock *sk, u16 index, unsigned char *data,
								u16 len)
=======
static int pin_code_neg_reply(struct sock *sk, struct hci_dev *hdev,
			      void *data, u16 len)
{
	struct mgmt_cp_pin_code_neg_reply *cp = data;

	BT_DBG("");

	return user_pairing_resp(sk, hdev, &cp->addr,
				MGMT_OP_PIN_CODE_NEG_REPLY,
				HCI_OP_PIN_CODE_NEG_REPLY, 0);
}

static int user_confirm_reply(struct sock *sk, struct hci_dev *hdev, void *data,
			      u16 len)
>>>>>>> android-4.9
{
	struct mgmt_cp_cancel_resolve_name *mgmt_cp = (void *) data;
	struct hci_cp_remote_name_req_cancel hci_cp;
	struct hci_dev *hdev;
	int err;

	BT_DBG("");

<<<<<<< HEAD
	if (len != sizeof(*mgmt_cp))
		return cmd_status(sk, index, MGMT_OP_CANCEL_RESOLVE_NAME,
								EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_CANCEL_RESOLVE_NAME,
								ENODEV);

	hci_dev_lock_bh(hdev);

	memset(&hci_cp, 0, sizeof(hci_cp));
	bacpy(&hci_cp.bdaddr, &mgmt_cp->bdaddr);
	err = hci_send_cmd(hdev, HCI_OP_REMOTE_NAME_REQ_CANCEL, sizeof(hci_cp),
								&hci_cp);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
=======
	if (len != sizeof(*cp))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_USER_CONFIRM_REPLY,
				       MGMT_STATUS_INVALID_PARAMS);

	return user_pairing_resp(sk, hdev, &cp->addr,
				 MGMT_OP_USER_CONFIRM_REPLY,
				 HCI_OP_USER_CONFIRM_REPLY, 0);
>>>>>>> android-4.9
}

static int set_connection_params(struct sock *sk, u16 index,
				unsigned char *data, u16 len)
{
	struct mgmt_cp_set_connection_params *cp = (void *) data;
	struct hci_dev *hdev;
	struct hci_conn *conn;
	int err;

	BT_DBG("");

<<<<<<< HEAD
	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_CONNECTION_PARAMS,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_CONNECTION_PARAMS,
									ENODEV);

	hci_dev_lock_bh(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->bdaddr);
	if (!conn) {
		err = cmd_status(sk, index, MGMT_OP_SET_CONNECTION_PARAMS,
								ENOTCONN);
		goto failed;
	}

	hci_le_conn_update(conn, le16_to_cpu(cp->interval_min),
				le16_to_cpu(cp->interval_max),
				le16_to_cpu(cp->slave_latency),
				le16_to_cpu(cp->timeout_multiplier));

	err = cmd_status(sk, index, MGMT_OP_SET_CONNECTION_PARAMS, 0);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
=======
	return user_pairing_resp(sk, hdev, &cp->addr,
				 MGMT_OP_USER_CONFIRM_NEG_REPLY,
				 HCI_OP_USER_CONFIRM_NEG_REPLY, 0);
>>>>>>> android-4.9
}

static int set_rssi_reporter(struct sock *sk, u16 index,
				unsigned char *data, u16 len)
{
	struct mgmt_cp_set_rssi_reporter *cp = (void *) data;
	struct hci_dev *hdev;
	struct hci_conn *conn;
	int err = 0;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_SET_RSSI_REPORTER,
								EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_RSSI_REPORTER,
							ENODEV);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->bdaddr);

	if (!conn) {
		err = cmd_status(sk, index, MGMT_OP_SET_RSSI_REPORTER,
						ENOTCONN);
		goto failed;
	}

	BT_DBG("updateOnThreshExceed %d ", cp->updateOnThreshExceed);
	hci_conn_set_rssi_reporter(conn, cp->rssi_threshold,
			__le16_to_cpu(cp->interval), cp->updateOnThreshExceed);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
=======
	return user_pairing_resp(sk, hdev, &cp->addr,
				 MGMT_OP_USER_PASSKEY_REPLY,
				 HCI_OP_USER_PASSKEY_REPLY, cp->passkey);
>>>>>>> android-4.9
}

static int unset_rssi_reporter(struct sock *sk, u16 index,
			unsigned char *data, u16 len)
{
	struct mgmt_cp_unset_rssi_reporter *cp = (void *) data;
	struct hci_dev *hdev;
	struct hci_conn *conn;
	int err = 0;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_UNSET_RSSI_REPORTER,
					EINVAL);

	hdev = hci_dev_get(index);

<<<<<<< HEAD
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_UNSET_RSSI_REPORTER,
					ENODEV);

	hci_dev_lock_bh(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->bdaddr);

	if (!conn) {
		err = cmd_status(sk, index, MGMT_OP_UNSET_RSSI_REPORTER,
					ENOTCONN);
		goto failed;
	}

	hci_conn_unset_rssi_reporter(conn);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int le_cancel_create_conn(struct sock *sk, u16 index,
	unsigned char *data, u16 len)
{
	struct mgmt_cp_le_cancel_create_conn *cp = (void *) data;
	struct hci_dev *hdev;
	int err = 0;

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_LE_CANCEL_CREATE_CONN,
							EINVAL);

	hdev = hci_dev_get(index);

	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_LE_CANCEL_CREATE_CONN,
							ENODEV);

	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_LE_CANCEL_CREATE_CONN,
						ENETDOWN);
		goto failed;
	}

	hci_le_cancel_create_connect(hdev, &cp->bdaddr);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

return err;
=======
	return user_pairing_resp(sk, hdev, &cp->addr,
				 MGMT_OP_USER_PASSKEY_NEG_REPLY,
				 HCI_OP_USER_PASSKEY_NEG_REPLY, 0);
}

static void adv_expire(struct hci_dev *hdev, u32 flags)
{
	struct adv_info *adv_instance;
	struct hci_request req;
	int err;

	adv_instance = hci_find_adv_instance(hdev, hdev->cur_adv_instance);
	if (!adv_instance)
		return;

	/* stop if current instance doesn't need to be changed */
	if (!(adv_instance->flags & flags))
		return;

	cancel_adv_timeout(hdev);

	adv_instance = hci_get_next_instance(hdev, adv_instance->instance);
	if (!adv_instance)
		return;

	hci_req_init(&req, hdev);
	err = __hci_req_schedule_adv_instance(&req, adv_instance->instance,
					      true);
	if (err)
		return;

	hci_req_run(&req, NULL);
}

static void set_name_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct mgmt_cp_set_local_name *cp;
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status 0x%02x", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_LOCAL_NAME, hdev);
	if (!cmd)
		goto unlock;

	cp = cmd->param;

	if (status) {
		mgmt_cmd_status(cmd->sk, hdev->id, MGMT_OP_SET_LOCAL_NAME,
			        mgmt_status(status));
	} else {
		mgmt_cmd_complete(cmd->sk, hdev->id, MGMT_OP_SET_LOCAL_NAME, 0,
				  cp, sizeof(*cp));

		if (hci_dev_test_flag(hdev, HCI_LE_ADV))
			adv_expire(hdev, MGMT_ADV_FLAG_LOCAL_NAME);
	}

	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
>>>>>>> android-4.9
}

static int set_local_name(struct sock *sk, u16 index, unsigned char *data,
								u16 len)
{
<<<<<<< HEAD
	struct mgmt_cp_set_local_name *mgmt_cp = (void *) data;
	struct hci_cp_write_local_name hci_cp;
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
=======
	struct mgmt_cp_set_local_name *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
>>>>>>> android-4.9
	int err;

	BT_DBG("");

<<<<<<< HEAD
	if (len != sizeof(*mgmt_cp))
		return cmd_status(sk, index, MGMT_OP_SET_LOCAL_NAME, EINVAL);
=======
	hci_dev_lock(hdev);

	/* If the old values are the same as the new ones just return a
	 * direct command complete event.
	 */
	if (!memcmp(hdev->dev_name, cp->name, sizeof(hdev->dev_name)) &&
	    !memcmp(hdev->short_name, cp->short_name,
		    sizeof(hdev->short_name))) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_LOCAL_NAME, 0,
					data, len);
		goto failed;
	}

	memcpy(hdev->short_name, cp->short_name, sizeof(hdev->short_name));

	if (!hdev_is_powered(hdev)) {
		memcpy(hdev->dev_name, cp->name, sizeof(hdev->dev_name));

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_LOCAL_NAME, 0,
					data, len);
		if (err < 0)
			goto failed;

		err = mgmt_limited_event(MGMT_EV_LOCAL_NAME_CHANGED, hdev, data,
					 len, HCI_MGMT_LOCAL_NAME_EVENTS, sk);
		ext_info_changed(hdev, sk);
>>>>>>> android-4.9

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_SET_LOCAL_NAME, ENODEV);

	hci_dev_lock_bh(hdev);

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_LOCAL_NAME, index, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

<<<<<<< HEAD
	memcpy(hci_cp.name, mgmt_cp->name, sizeof(hci_cp.name));
	err = hci_send_cmd(hdev, HCI_OP_WRITE_LOCAL_NAME, sizeof(hci_cp),
								&hci_cp);
=======
	memcpy(hdev->dev_name, cp->name, sizeof(hdev->dev_name));

	hci_req_init(&req, hdev);

	if (lmp_bredr_capable(hdev)) {
		__hci_req_update_name(&req);
		__hci_req_update_eir(&req);
	}

	/* The name is stored in the scan response data and so
	 * no need to udpate the advertising data here.
	 */
	if (lmp_le_capable(hdev) && hci_dev_test_flag(hdev, HCI_ADVERTISING))
		__hci_req_update_scan_rsp_data(&req, hdev->cur_adv_instance);

	err = hci_req_run(&req, set_name_complete);
>>>>>>> android-4.9
	if (err < 0)
		mgmt_pending_remove(cmd);

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

<<<<<<< HEAD
static void discovery_rsp(struct pending_cmd *cmd, void *data)
{
	struct mgmt_mode ev;

	BT_DBG("");
	if (cmd->opcode == MGMT_OP_START_DISCOVERY) {
		ev.val = 1;
		cmd_status(cmd->sk, cmd->index, MGMT_OP_START_DISCOVERY, 0);
	} else {
		ev.val = 0;
		cmd_complete(cmd->sk, cmd->index, MGMT_OP_STOP_DISCOVERY,
								NULL, 0);
		if (cmd->opcode == MGMT_OP_STOP_DISCOVERY) {
			struct hci_dev *hdev = hci_dev_get(cmd->index);
			if (hdev) {
				del_timer(&hdev->disco_le_timer);
				del_timer(&hdev->disco_timer);
				hci_dev_put(hdev);
			}
		}
	}

	mgmt_event(MGMT_EV_DISCOVERING, cmd->index, &ev, sizeof(ev), NULL);
=======
static int set_appearance(struct sock *sk, struct hci_dev *hdev, void *data,
			  u16 len)
{
	struct mgmt_cp_set_appearance *cp = data;
	u16 apperance;
	int err;

	BT_DBG("");

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_APPEARANCE,
				       MGMT_STATUS_NOT_SUPPORTED);

	apperance = le16_to_cpu(cp->appearance);

	hci_dev_lock(hdev);

	if (hdev->appearance != apperance) {
		hdev->appearance = apperance;

		if (hci_dev_test_flag(hdev, HCI_LE_ADV))
			adv_expire(hdev, MGMT_ADV_FLAG_APPEARANCE);

		ext_info_changed(hdev, sk);
	}

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_APPEARANCE, 0, NULL,
				0);

	hci_dev_unlock(hdev);

	return err;
}

static void read_local_oob_data_complete(struct hci_dev *hdev, u8 status,
				         u16 opcode, struct sk_buff *skb)
{
	struct mgmt_rp_read_local_oob_data mgmt_rp;
	size_t rp_size = sizeof(mgmt_rp);
	struct mgmt_pending_cmd *cmd;

	BT_DBG("%s status %u", hdev->name, status);

	cmd = pending_find(MGMT_OP_READ_LOCAL_OOB_DATA, hdev);
	if (!cmd)
		return;

	if (status || !skb) {
		mgmt_cmd_status(cmd->sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_DATA,
				status ? mgmt_status(status) : MGMT_STATUS_FAILED);
		goto remove;
	}

	memset(&mgmt_rp, 0, sizeof(mgmt_rp));

	if (opcode == HCI_OP_READ_LOCAL_OOB_DATA) {
		struct hci_rp_read_local_oob_data *rp = (void *) skb->data;

		if (skb->len < sizeof(*rp)) {
			mgmt_cmd_status(cmd->sk, hdev->id,
					MGMT_OP_READ_LOCAL_OOB_DATA,
					MGMT_STATUS_FAILED);
			goto remove;
		}

		memcpy(mgmt_rp.hash192, rp->hash, sizeof(rp->hash));
		memcpy(mgmt_rp.rand192, rp->rand, sizeof(rp->rand));

		rp_size -= sizeof(mgmt_rp.hash256) + sizeof(mgmt_rp.rand256);
	} else {
		struct hci_rp_read_local_oob_ext_data *rp = (void *) skb->data;

		if (skb->len < sizeof(*rp)) {
			mgmt_cmd_status(cmd->sk, hdev->id,
					MGMT_OP_READ_LOCAL_OOB_DATA,
					MGMT_STATUS_FAILED);
			goto remove;
		}

		memcpy(mgmt_rp.hash192, rp->hash192, sizeof(rp->hash192));
		memcpy(mgmt_rp.rand192, rp->rand192, sizeof(rp->rand192));

		memcpy(mgmt_rp.hash256, rp->hash256, sizeof(rp->hash256));
		memcpy(mgmt_rp.rand256, rp->rand256, sizeof(rp->rand256));
	}

	mgmt_cmd_complete(cmd->sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_DATA,
			  MGMT_STATUS_SUCCESS, &mgmt_rp, rp_size);

remove:
	mgmt_pending_remove(cmd);
}

static int read_local_oob_data(struct sock *sk, struct hci_dev *hdev,
			       void *data, u16 data_len)
{
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_DATA,
				      MGMT_STATUS_NOT_POWERED);
		goto unlock;
	}

	if (!lmp_ssp_capable(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_DATA,
				      MGMT_STATUS_NOT_SUPPORTED);
		goto unlock;
	}

	if (pending_find(MGMT_OP_READ_LOCAL_OOB_DATA, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_DATA,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}
>>>>>>> android-4.9

	list_del(&cmd->list);

<<<<<<< HEAD
	mgmt_pending_free(cmd);
}
=======
	hci_req_init(&req, hdev);

	if (bredr_sc_enabled(hdev))
		hci_req_add(&req, HCI_OP_READ_LOCAL_OOB_EXT_DATA, 0, NULL);
	else
		hci_req_add(&req, HCI_OP_READ_LOCAL_OOB_DATA, 0, NULL);

	err = hci_req_run_skb(&req, read_local_oob_data_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);
>>>>>>> android-4.9

void mgmt_inquiry_started(u16 index)
{
	BT_DBG("");
	mgmt_pending_foreach(MGMT_OP_START_DISCOVERY, index,
						discovery_rsp, NULL);
}

void mgmt_inquiry_complete_evt(u16 index, u8 status)
{
<<<<<<< HEAD
	struct hci_dev *hdev;
	struct hci_cp_le_set_scan_enable le_cp = {1, 0};
	struct mgmt_mode cp = {0};
	int err = -1;

	hdev = hci_dev_get(index);
=======
	struct mgmt_addr_info *addr = data;
	int err;
>>>>>>> android-4.9

	if (hdev)
		BT_DBG("disco_state: %d", hdev->disco_state);

<<<<<<< HEAD
	if (!hdev || !lmp_le_capable(hdev)) {

		mgmt_pending_foreach(MGMT_OP_STOP_DISCOVERY, index,
						discovery_terminated, NULL);

		mgmt_event(MGMT_EV_DISCOVERING, index, &cp, sizeof(cp), NULL);

		hdev->disco_state = SCAN_IDLE;

		if (hdev)
			goto done;
		else
			return;
	}

	if (hdev->disco_state != SCAN_IDLE) {
		err = hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_ENABLE,
						sizeof(le_cp), &le_cp);
		if (err >= 0) {
			mod_timer(&hdev->disco_le_timer, jiffies +
				msecs_to_jiffies(hdev->disco_int_phase * 1000));
			hdev->disco_state = SCAN_LE;
		} else
			hdev->disco_state = SCAN_IDLE;
	}

	if (hdev->disco_state == SCAN_IDLE)
		mgmt_event(MGMT_EV_DISCOVERING, index, &cp, sizeof(cp), NULL);
=======
	if (!bdaddr_type_is_valid(addr->type))
		return mgmt_cmd_complete(sk, hdev->id,
					 MGMT_OP_ADD_REMOTE_OOB_DATA,
					 MGMT_STATUS_INVALID_PARAMS,
					 addr, sizeof(*addr));

	hci_dev_lock(hdev);

	if (len == MGMT_ADD_REMOTE_OOB_DATA_SIZE) {
		struct mgmt_cp_add_remote_oob_data *cp = data;
		u8 status;

		if (cp->addr.type != BDADDR_BREDR) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_ADD_REMOTE_OOB_DATA,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		err = hci_add_remote_oob_data(hdev, &cp->addr.bdaddr,
					      cp->addr.type, cp->hash,
					      cp->rand, NULL, NULL);
		if (err < 0)
			status = MGMT_STATUS_FAILED;
		else
			status = MGMT_STATUS_SUCCESS;

		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_ADD_REMOTE_OOB_DATA, status,
					&cp->addr, sizeof(cp->addr));
	} else if (len == MGMT_ADD_REMOTE_OOB_EXT_DATA_SIZE) {
		struct mgmt_cp_add_remote_oob_ext_data *cp = data;
		u8 *rand192, *hash192, *rand256, *hash256;
		u8 status;

		if (bdaddr_type_is_le(cp->addr.type)) {
			/* Enforce zero-valued 192-bit parameters as
			 * long as legacy SMP OOB isn't implemented.
			 */
			if (memcmp(cp->rand192, ZERO_KEY, 16) ||
			    memcmp(cp->hash192, ZERO_KEY, 16)) {
				err = mgmt_cmd_complete(sk, hdev->id,
							MGMT_OP_ADD_REMOTE_OOB_DATA,
							MGMT_STATUS_INVALID_PARAMS,
							addr, sizeof(*addr));
				goto unlock;
			}

			rand192 = NULL;
			hash192 = NULL;
		} else {
			/* In case one of the P-192 values is set to zero,
			 * then just disable OOB data for P-192.
			 */
			if (!memcmp(cp->rand192, ZERO_KEY, 16) ||
			    !memcmp(cp->hash192, ZERO_KEY, 16)) {
				rand192 = NULL;
				hash192 = NULL;
			} else {
				rand192 = cp->rand192;
				hash192 = cp->hash192;
			}
		}

		/* In case one of the P-256 values is set to zero, then just
		 * disable OOB data for P-256.
		 */
		if (!memcmp(cp->rand256, ZERO_KEY, 16) ||
		    !memcmp(cp->hash256, ZERO_KEY, 16)) {
			rand256 = NULL;
			hash256 = NULL;
		} else {
			rand256 = cp->rand256;
			hash256 = cp->hash256;
		}

		err = hci_add_remote_oob_data(hdev, &cp->addr.bdaddr,
					      cp->addr.type, hash192, rand192,
					      hash256, rand256);
		if (err < 0)
			status = MGMT_STATUS_FAILED;
		else
			status = MGMT_STATUS_SUCCESS;

		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_ADD_REMOTE_OOB_DATA,
					status, &cp->addr, sizeof(cp->addr));
	} else {
		BT_ERR("add_remote_oob_data: invalid length of %u bytes", len);
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_REMOTE_OOB_DATA,
				      MGMT_STATUS_INVALID_PARAMS);
	}
>>>>>>> android-4.9

	if (err < 0)
		mgmt_pending_foreach(MGMT_OP_STOP_DISCOVERY, index,
						discovery_terminated, NULL);

done:
	hci_dev_put(hdev);
}

<<<<<<< HEAD
void mgmt_disco_timeout(unsigned long data)
=======
static int remove_remote_oob_data(struct sock *sk, struct hci_dev *hdev,
				  void *data, u16 len)
>>>>>>> android-4.9
{
	struct hci_dev *hdev = (void *) data;
	struct pending_cmd *cmd;
	struct mgmt_mode cp = {0};

	BT_DBG("hci%d", hdev->id);

<<<<<<< HEAD
	hdev = hci_dev_get(hdev->id);

	if (!hdev)
		return;

	hci_dev_lock_bh(hdev);
	del_timer(&hdev->disco_le_timer);

	if (hdev->disco_state != SCAN_IDLE) {
		struct hci_cp_le_set_scan_enable le_cp = {0, 0};

		if (test_bit(HCI_UP, &hdev->flags)) {
			if (hdev->disco_state == SCAN_LE)
				hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_ENABLE,
							sizeof(le_cp), &le_cp);
			else
				hci_send_cmd(hdev, HCI_OP_INQUIRY_CANCEL, 0,
									 NULL);
		}
		hdev->disco_state = SCAN_IDLE;
	}

	mgmt_event(MGMT_EV_DISCOVERING, hdev->id, &cp, sizeof(cp), NULL);

	cmd = mgmt_pending_find(MGMT_OP_STOP_DISCOVERY, hdev->id);
	if (cmd)
		mgmt_pending_remove(cmd);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
}

void mgmt_disco_le_timeout(unsigned long data)
{
	struct hci_dev *hdev = (void *)data;
	struct hci_cp_le_set_scan_enable le_cp = {0, 0};

	BT_DBG("hci%d", hdev->id);
=======
	if (cp->addr.type != BDADDR_BREDR)
		return mgmt_cmd_complete(sk, hdev->id,
					 MGMT_OP_REMOVE_REMOTE_OOB_DATA,
					 MGMT_STATUS_INVALID_PARAMS,
					 &cp->addr, sizeof(cp->addr));

	hci_dev_lock(hdev);

	if (!bacmp(&cp->addr.bdaddr, BDADDR_ANY)) {
		hci_remote_oob_data_clear(hdev);
		status = MGMT_STATUS_SUCCESS;
		goto done;
	}

	err = hci_remove_remote_oob_data(hdev, &cp->addr.bdaddr, cp->addr.type);
	if (err < 0)
		status = MGMT_STATUS_INVALID_PARAMS;
	else
		status = MGMT_STATUS_SUCCESS;

done:
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_REMOVE_REMOTE_OOB_DATA,
				status, &cp->addr, sizeof(cp->addr));

	hci_dev_unlock(hdev);
	return err;
}

void mgmt_start_discovery_complete(struct hci_dev *hdev, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status %d", status);
>>>>>>> android-4.9

	hdev = hci_dev_get(hdev->id);

<<<<<<< HEAD
	if (!hdev)
		return;

	hci_dev_lock_bh(hdev);

	if (test_bit(HCI_UP, &hdev->flags)) {
		if (hdev->disco_state == SCAN_LE)
			hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_ENABLE,
					sizeof(le_cp), &le_cp);

	/* re-start BR scan */
		if (hdev->disco_state != SCAN_IDLE) {
			struct hci_cp_inquiry cp = {{0x33, 0x8b, 0x9e}, 4, 0};
			hdev->disco_int_phase *= 2;
			hdev->disco_int_count = 0;
			cp.num_rsp = (u8) hdev->disco_int_phase;
			hci_send_cmd(hdev, HCI_OP_INQUIRY, sizeof(cp), &cp);
			hdev->disco_state = SCAN_BR;
		}
	}

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
}

static int start_discovery(struct sock *sk, u16 index)
{
	struct hci_cp_inquiry cp = {{0x33, 0x8b, 0x9e}, 8, 0};
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
=======
	cmd = pending_find(MGMT_OP_START_DISCOVERY, hdev);
	if (!cmd)
		cmd = pending_find(MGMT_OP_START_SERVICE_DISCOVERY, hdev);

	if (!cmd)
		cmd = pending_find(MGMT_OP_START_LIMITED_DISCOVERY, hdev);

	if (cmd) {
		cmd->cmd_complete(cmd, mgmt_status(status));
		mgmt_pending_remove(cmd);
	}

	hci_dev_unlock(hdev);
}

static bool discovery_type_is_valid(struct hci_dev *hdev, uint8_t type,
				    uint8_t *mgmt_status)
{
	switch (type) {
	case DISCOV_TYPE_LE:
		*mgmt_status = mgmt_le_support(hdev);
		if (*mgmt_status)
			return false;
		break;
	case DISCOV_TYPE_INTERLEAVED:
		*mgmt_status = mgmt_le_support(hdev);
		if (*mgmt_status)
			return false;
		/* Intentional fall-through */
	case DISCOV_TYPE_BREDR:
		*mgmt_status = mgmt_bredr_support(hdev);
		if (*mgmt_status)
			return false;
		break;
	default:
		*mgmt_status = MGMT_STATUS_INVALID_PARAMS;
		return false;
	}

	return true;
}

static int start_discovery_internal(struct sock *sk, struct hci_dev *hdev,
				    u16 op, void *data, u16 len)
{
	struct mgmt_cp_start_discovery *cp = data;
	struct mgmt_pending_cmd *cmd;
	u8 status;
>>>>>>> android-4.9
	int err;

	BT_DBG("");

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_START_DISCOVERY, ENODEV);

<<<<<<< HEAD
	BT_DBG("disco_state: %d", hdev->disco_state);
	hci_dev_lock_bh(hdev);

	if (hdev->disco_state && timer_pending(&hdev->disco_timer)) {
		err = -EBUSY;
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_START_DISCOVERY, index, NULL, 0);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	/* If LE Capable, we will alternate between BR/EDR and LE */
	if (lmp_le_capable(hdev)) {
		struct hci_cp_le_set_scan_parameters le_cp;

		/* Shorten BR scan params */
		cp.num_rsp = 1;
		cp.length /= 2;

		/* Setup LE scan params */
		memset(&le_cp, 0, sizeof(le_cp));
		le_cp.type = 0x01;		/* Active scanning */
		/* The recommended value for scan interval and window is
		 * 11.25 msec. It is calculated by: time = n * 0.625 msec */
		le_cp.interval = cpu_to_le16(0x0012);
		le_cp.window = cpu_to_le16(0x0012);
		le_cp.own_bdaddr_type = 0;	/* Public address */
		le_cp.filter = 0;		/* Accept all adv packets */

		hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_PARAMETERS,
						sizeof(le_cp), &le_cp);
	}

	err = hci_send_cmd(hdev, HCI_OP_INQUIRY, sizeof(cp), &cp);

	if (err < 0) {
		mgmt_pending_remove(cmd);
		hdev->disco_state = SCAN_IDLE;
	} else if (lmp_le_capable(hdev)) {
		cmd = mgmt_pending_find(MGMT_OP_STOP_DISCOVERY, index);
		if (!cmd)
			mgmt_pending_add(sk, MGMT_OP_STOP_DISCOVERY, index,
								NULL, 0);
		hdev->disco_int_phase = 1;
		hdev->disco_int_count = 0;
		hdev->disco_state = SCAN_BR;
		del_timer(&hdev->disco_le_timer);
		del_timer(&hdev->disco_timer);
		mod_timer(&hdev->disco_timer,
				jiffies + msecs_to_jiffies(20000));
	} else
		hdev->disco_state = SCAN_BR;
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, op,
					MGMT_STATUS_NOT_POWERED,
					&cp->type, sizeof(cp->type));
		goto failed;
	}

	if (hdev->discovery.state != DISCOVERY_STOPPED ||
	    hci_dev_test_flag(hdev, HCI_PERIODIC_INQ)) {
		err = mgmt_cmd_complete(sk, hdev->id, op, MGMT_STATUS_BUSY,
					&cp->type, sizeof(cp->type));
		goto failed;
	}

	if (!discovery_type_is_valid(hdev, cp->type, &status)) {
		err = mgmt_cmd_complete(sk, hdev->id, op, status,
					&cp->type, sizeof(cp->type));
		goto failed;
	}

	/* Clear the discovery filter first to free any previously
	 * allocated memory for the UUID list.
	 */
	hci_discovery_filter_clear(hdev);

	hdev->discovery.type = cp->type;
	hdev->discovery.report_invalid_rssi = false;
	if (op == MGMT_OP_START_LIMITED_DISCOVERY)
		hdev->discovery.limited = true;
	else
		hdev->discovery.limited = false;

	cmd = mgmt_pending_add(sk, op, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	cmd->cmd_complete = generic_cmd_complete;

	hci_discovery_set_state(hdev, DISCOVERY_STARTING);
	queue_work(hdev->req_workqueue, &hdev->discov_update);
	err = 0;
>>>>>>> android-4.9

failed:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	if (err < 0)
		return cmd_status(sk, index, MGMT_OP_START_DISCOVERY, -err);

	return err;
}

<<<<<<< HEAD
static int stop_discovery(struct sock *sk, u16 index)
{
	struct hci_cp_le_set_scan_enable le_cp = {0, 0};
	struct mgmt_mode mode_cp = {0};
	struct hci_dev *hdev;
	struct pending_cmd *cmd = NULL;
	int err = -EPERM;
	u8 state;
=======
static int start_discovery(struct sock *sk, struct hci_dev *hdev,
			   void *data, u16 len)
{
	return start_discovery_internal(sk, hdev, MGMT_OP_START_DISCOVERY,
					data, len);
}

static int start_limited_discovery(struct sock *sk, struct hci_dev *hdev,
				   void *data, u16 len)
{
	return start_discovery_internal(sk, hdev,
					MGMT_OP_START_LIMITED_DISCOVERY,
					data, len);
}

static int service_discovery_cmd_complete(struct mgmt_pending_cmd *cmd,
					  u8 status)
{
	return mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode, status,
				 cmd->param, 1);
}

static int start_service_discovery(struct sock *sk, struct hci_dev *hdev,
				   void *data, u16 len)
{
	struct mgmt_cp_start_service_discovery *cp = data;
	struct mgmt_pending_cmd *cmd;
	const u16 max_uuid_count = ((U16_MAX - sizeof(*cp)) / 16);
	u16 uuid_count, expected_len;
	u8 status;
	int err;
>>>>>>> android-4.9

	BT_DBG("");

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_STOP_DISCOVERY, ENODEV);

<<<<<<< HEAD
	BT_DBG("disco_state: %d", hdev->disco_state);

	hci_dev_lock_bh(hdev);

	state = hdev->disco_state;
	hdev->disco_state = SCAN_IDLE;
	del_timer(&hdev->disco_le_timer);
	del_timer(&hdev->disco_timer);

	if (state == SCAN_LE) {
		err = hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_ENABLE,
							sizeof(le_cp), &le_cp);
		if (err >= 0) {
			mgmt_pending_foreach(MGMT_OP_STOP_DISCOVERY, index,
						discovery_terminated, NULL);

			err = cmd_complete(sk, index, MGMT_OP_STOP_DISCOVERY,
								NULL, 0);
		}
	} else if (state == SCAN_BR)
		err = hci_send_cmd(hdev, HCI_OP_INQUIRY_CANCEL, 0, NULL);

	cmd = mgmt_pending_find(MGMT_OP_STOP_DISCOVERY, index);
	if (err < 0 && cmd)
		mgmt_pending_remove(cmd);

	mgmt_event(MGMT_EV_DISCOVERING, index, &mode_cp, sizeof(mode_cp), NULL);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	if (err < 0)
		return cmd_status(sk, index, MGMT_OP_STOP_DISCOVERY, -err);
	else
		return err;
=======
	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_START_SERVICE_DISCOVERY,
					MGMT_STATUS_NOT_POWERED,
					&cp->type, sizeof(cp->type));
		goto failed;
	}

	if (hdev->discovery.state != DISCOVERY_STOPPED ||
	    hci_dev_test_flag(hdev, HCI_PERIODIC_INQ)) {
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_START_SERVICE_DISCOVERY,
					MGMT_STATUS_BUSY, &cp->type,
					sizeof(cp->type));
		goto failed;
	}

	uuid_count = __le16_to_cpu(cp->uuid_count);
	if (uuid_count > max_uuid_count) {
		BT_ERR("service_discovery: too big uuid_count value %u",
		       uuid_count);
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_START_SERVICE_DISCOVERY,
					MGMT_STATUS_INVALID_PARAMS, &cp->type,
					sizeof(cp->type));
		goto failed;
	}

	expected_len = sizeof(*cp) + uuid_count * 16;
	if (expected_len != len) {
		BT_ERR("service_discovery: expected %u bytes, got %u bytes",
		       expected_len, len);
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_START_SERVICE_DISCOVERY,
					MGMT_STATUS_INVALID_PARAMS, &cp->type,
					sizeof(cp->type));
		goto failed;
	}

	if (!discovery_type_is_valid(hdev, cp->type, &status)) {
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_START_SERVICE_DISCOVERY,
					status, &cp->type, sizeof(cp->type));
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_START_SERVICE_DISCOVERY,
			       hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	cmd->cmd_complete = service_discovery_cmd_complete;

	/* Clear the discovery filter first to free any previously
	 * allocated memory for the UUID list.
	 */
	hci_discovery_filter_clear(hdev);

	hdev->discovery.result_filtering = true;
	hdev->discovery.type = cp->type;
	hdev->discovery.rssi = cp->rssi;
	hdev->discovery.uuid_count = uuid_count;

	if (uuid_count > 0) {
		hdev->discovery.uuids = kmemdup(cp->uuids, uuid_count * 16,
						GFP_KERNEL);
		if (!hdev->discovery.uuids) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_START_SERVICE_DISCOVERY,
						MGMT_STATUS_FAILED,
						&cp->type, sizeof(cp->type));
			mgmt_pending_remove(cmd);
			goto failed;
		}
	}

	hci_discovery_set_state(hdev, DISCOVERY_STARTING);
	queue_work(hdev->req_workqueue, &hdev->discov_update);
	err = 0;

failed:
	hci_dev_unlock(hdev);
	return err;
}

void mgmt_stop_discovery_complete(struct hci_dev *hdev, u8 status)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status %d", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_STOP_DISCOVERY, hdev);
	if (cmd) {
		cmd->cmd_complete(cmd, mgmt_status(status));
		mgmt_pending_remove(cmd);
	}

	hci_dev_unlock(hdev);
}

static int stop_discovery(struct sock *sk, struct hci_dev *hdev, void *data,
			  u16 len)
{
	struct mgmt_cp_stop_discovery *mgmt_cp = data;
	struct mgmt_pending_cmd *cmd;
	int err;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (!hci_discovery_active(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_STOP_DISCOVERY,
					MGMT_STATUS_REJECTED, &mgmt_cp->type,
					sizeof(mgmt_cp->type));
		goto unlock;
	}

	if (hdev->discovery.type != mgmt_cp->type) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_STOP_DISCOVERY,
					MGMT_STATUS_INVALID_PARAMS,
					&mgmt_cp->type, sizeof(mgmt_cp->type));
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_STOP_DISCOVERY, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	cmd->cmd_complete = generic_cmd_complete;

	hci_discovery_set_state(hdev, DISCOVERY_STOPPING);
	queue_work(hdev->req_workqueue, &hdev->discov_update);
	err = 0;

unlock:
	hci_dev_unlock(hdev);
	return err;
>>>>>>> android-4.9
}

static int read_local_oob_data(struct sock *sk, u16 index)
{
	struct hci_dev *hdev;
	struct pending_cmd *cmd;
	int err;

	BT_DBG("hci%u", index);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_READ_LOCAL_OOB_DATA,
									ENODEV);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = cmd_status(sk, index, MGMT_OP_READ_LOCAL_OOB_DATA,
								ENETDOWN);
		goto unlock;
=======
	if (!hci_discovery_active(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_CONFIRM_NAME,
					MGMT_STATUS_FAILED, &cp->addr,
					sizeof(cp->addr));
		goto failed;
	}

	e = hci_inquiry_cache_lookup_unknown(hdev, &cp->addr.bdaddr);
	if (!e) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_CONFIRM_NAME,
					MGMT_STATUS_INVALID_PARAMS, &cp->addr,
					sizeof(cp->addr));
		goto failed;
>>>>>>> android-4.9
	}

	if (!(hdev->features[6] & LMP_SIMPLE_PAIR)) {
		err = cmd_status(sk, index, MGMT_OP_READ_LOCAL_OOB_DATA,
								EOPNOTSUPP);
		goto unlock;
	}

<<<<<<< HEAD
	if (mgmt_pending_find(MGMT_OP_READ_LOCAL_OOB_DATA, index)) {
		err = cmd_status(sk, index, MGMT_OP_READ_LOCAL_OOB_DATA, EBUSY);
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_READ_LOCAL_OOB_DATA, index, NULL, 0);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	err = hci_send_cmd(hdev, HCI_OP_READ_LOCAL_OOB_DATA, 0, NULL);
	if (err < 0)
		mgmt_pending_remove(cmd);
=======
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_CONFIRM_NAME, 0,
				&cp->addr, sizeof(cp->addr));

failed:
	hci_dev_unlock(hdev);
	return err;
}

static int block_device(struct sock *sk, struct hci_dev *hdev, void *data,
			u16 len)
{
	struct mgmt_cp_block_device *cp = data;
	u8 status;
	int err;

	BT_DBG("%s", hdev->name);

	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_BLOCK_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &cp->addr, sizeof(cp->addr));

	hci_dev_lock(hdev);

	err = hci_bdaddr_list_add(&hdev->blacklist, &cp->addr.bdaddr,
				  cp->addr.type);
	if (err < 0) {
		status = MGMT_STATUS_FAILED;
		goto done;
	}

	mgmt_event(MGMT_EV_DEVICE_BLOCKED, hdev, &cp->addr, sizeof(cp->addr),
		   sk);
	status = MGMT_STATUS_SUCCESS;

done:
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_BLOCK_DEVICE, status,
				&cp->addr, sizeof(cp->addr));
>>>>>>> android-4.9

unlock:
	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

static int add_remote_oob_data(struct sock *sk, u16 index, unsigned char *data,
									u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_add_remote_oob_data *cp = (void *) data;
	int err;

	BT_DBG("hci%u ", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_ADD_REMOTE_OOB_DATA,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_ADD_REMOTE_OOB_DATA,
									ENODEV);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);

	err = hci_add_remote_oob_data(hdev, &cp->bdaddr, cp->hash,
								cp->randomizer);
	if (err < 0)
		err = cmd_status(sk, index, MGMT_OP_ADD_REMOTE_OOB_DATA, -err);
	else
		err = cmd_complete(sk, index, MGMT_OP_ADD_REMOTE_OOB_DATA, NULL,
									0);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);
=======
	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNBLOCK_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &cp->addr, sizeof(cp->addr));

	hci_dev_lock(hdev);

	err = hci_bdaddr_list_del(&hdev->blacklist, &cp->addr.bdaddr,
				  cp->addr.type);
	if (err < 0) {
		status = MGMT_STATUS_INVALID_PARAMS;
		goto done;
	}

	mgmt_event(MGMT_EV_DEVICE_UNBLOCKED, hdev, &cp->addr, sizeof(cp->addr),
		   sk);
	status = MGMT_STATUS_SUCCESS;

done:
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_UNBLOCK_DEVICE, status,
				&cp->addr, sizeof(cp->addr));

	hci_dev_unlock(hdev);
>>>>>>> android-4.9

	return err;
}

<<<<<<< HEAD
static int remove_remote_oob_data(struct sock *sk, u16 index,
						unsigned char *data, u16 len)
{
	struct hci_dev *hdev;
	struct mgmt_cp_remove_remote_oob_data *cp = (void *) data;
=======
static int set_device_id(struct sock *sk, struct hci_dev *hdev, void *data,
			 u16 len)
{
	struct mgmt_cp_set_device_id *cp = data;
	struct hci_request req;
>>>>>>> android-4.9
	int err;
	__u16 source;

<<<<<<< HEAD
	BT_DBG("hci%u ", index);

	if (len != sizeof(*cp))
		return cmd_status(sk, index, MGMT_OP_REMOVE_REMOTE_OOB_DATA,
									EINVAL);

	hdev = hci_dev_get(index);
	if (!hdev)
		return cmd_status(sk, index, MGMT_OP_REMOVE_REMOTE_OOB_DATA,
									ENODEV);

	hci_dev_lock_bh(hdev);

	err = hci_remove_remote_oob_data(hdev, &cp->bdaddr);
	if (err < 0)
		err = cmd_status(sk, index, MGMT_OP_REMOVE_REMOTE_OOB_DATA,
									-err);
	else
		err = cmd_complete(sk, index, MGMT_OP_REMOVE_REMOTE_OOB_DATA,
								NULL, 0);

	hci_dev_unlock_bh(hdev);
	hci_dev_put(hdev);

	return err;
}

int mgmt_control(struct sock *sk, struct msghdr *msg, size_t msglen)
{
	unsigned char *buf;
	struct mgmt_hdr *hdr;
	u16 opcode, index, len;
=======
	BT_DBG("%s", hdev->name);

	source = __le16_to_cpu(cp->source);

	if (source > 0x0002)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DEVICE_ID,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	hdev->devid_source = source;
	hdev->devid_vendor = __le16_to_cpu(cp->vendor);
	hdev->devid_product = __le16_to_cpu(cp->product);
	hdev->devid_version = __le16_to_cpu(cp->version);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_DEVICE_ID, 0,
				NULL, 0);

	hci_req_init(&req, hdev);
	__hci_req_update_eir(&req);
	hci_req_run(&req, NULL);

	hci_dev_unlock(hdev);

	return err;
}

static void enable_advertising_instance(struct hci_dev *hdev, u8 status,
					u16 opcode)
{
	BT_DBG("status %d", status);
}

static void set_advertising_complete(struct hci_dev *hdev, u8 status,
				     u16 opcode)
{
	struct cmd_lookup match = { NULL, hdev };
	struct hci_request req;
	u8 instance;
	struct adv_info *adv_instance;
	int err;

	hci_dev_lock(hdev);

	if (status) {
		u8 mgmt_err = mgmt_status(status);

		mgmt_pending_foreach(MGMT_OP_SET_ADVERTISING, hdev,
				     cmd_status_rsp, &mgmt_err);
		goto unlock;
	}

	if (hci_dev_test_flag(hdev, HCI_LE_ADV))
		hci_dev_set_flag(hdev, HCI_ADVERTISING);
	else
		hci_dev_clear_flag(hdev, HCI_ADVERTISING);

	mgmt_pending_foreach(MGMT_OP_SET_ADVERTISING, hdev, settings_rsp,
			     &match);

	new_settings(hdev, match.sk);

	if (match.sk)
		sock_put(match.sk);

	/* If "Set Advertising" was just disabled and instance advertising was
	 * set up earlier, then re-enable multi-instance advertising.
	 */
	if (hci_dev_test_flag(hdev, HCI_ADVERTISING) ||
	    list_empty(&hdev->adv_instances))
		goto unlock;

	instance = hdev->cur_adv_instance;
	if (!instance) {
		adv_instance = list_first_entry_or_null(&hdev->adv_instances,
							struct adv_info, list);
		if (!adv_instance)
			goto unlock;

		instance = adv_instance->instance;
	}

	hci_req_init(&req, hdev);

	err = __hci_req_schedule_adv_instance(&req, instance, true);

	if (!err)
		err = hci_req_run(&req, enable_advertising_instance);

	if (err)
		BT_ERR("Failed to re-configure advertising");

unlock:
	hci_dev_unlock(hdev);
}

static int set_advertising(struct sock *sk, struct hci_dev *hdev, void *data,
			   u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	u8 val, status;
>>>>>>> android-4.9
	int err;

	BT_DBG("request for %s", hdev->name);

	status = mgmt_le_support(hdev);
	if (status)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_ADVERTISING,
				       status);

	if (cp->val != 0x00 && cp->val != 0x01 && cp->val != 0x02)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_ADVERTISING,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

<<<<<<< HEAD
	hdr = (struct mgmt_hdr *) buf;
	opcode = get_unaligned_le16(&hdr->opcode);
	index = get_unaligned_le16(&hdr->index);
	len = get_unaligned_le16(&hdr->len);
=======
	val = !!cp->val;
>>>>>>> android-4.9

	/* The following conditions are ones which mean that we should
	 * not do any HCI communication but directly send a mgmt
	 * response to user space (after toggling the flag if
	 * necessary).
	 */
	if (!hdev_is_powered(hdev) ||
	    (val == hci_dev_test_flag(hdev, HCI_ADVERTISING) &&
	     (cp->val == 0x02) == hci_dev_test_flag(hdev, HCI_ADVERTISING_CONNECTABLE)) ||
	    hci_conn_num(hdev, LE_LINK) > 0 ||
	    (hci_dev_test_flag(hdev, HCI_LE_SCAN) &&
	     hdev->le_scan_type == LE_SCAN_ACTIVE)) {
		bool changed;

<<<<<<< HEAD
	BT_DBG("got opcode %x", opcode);
	switch (opcode) {
	case MGMT_OP_READ_VERSION:
		err = read_version(sk);
		break;
	case MGMT_OP_READ_INDEX_LIST:
		err = read_index_list(sk);
		break;
	case MGMT_OP_READ_INFO:
		err = read_controller_info(sk, index);
		break;
	case MGMT_OP_SET_POWERED:
		err = set_powered(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_DISCOVERABLE:
		err = set_discoverable(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_LIMIT_DISCOVERABLE:
		err = set_limited_discoverable(sk, index, buf + sizeof(*hdr),
									len);
		break;
	case MGMT_OP_SET_CONNECTABLE:
		err = set_connectable(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_PAIRABLE:
		err = set_pairable(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_ADD_UUID:
		err = add_uuid(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_REMOVE_UUID:
		err = remove_uuid(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_DEV_CLASS:
		err = set_dev_class(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_SERVICE_CACHE:
		err = set_service_cache(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_LOAD_KEYS:
		err = load_keys(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_REMOVE_KEY:
		err = remove_key(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_DISCONNECT:
		err = disconnect(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_GET_CONNECTIONS:
		err = get_connections(sk, index);
		break;
	case MGMT_OP_PIN_CODE_REPLY:
		err = pin_code_reply(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_PIN_CODE_NEG_REPLY:
		err = pin_code_neg_reply(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_IO_CAPABILITY:
		err = set_io_capability(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_PAIR_DEVICE:
		err = pair_device(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_USER_CONFIRM_REPLY:
	case MGMT_OP_USER_PASSKEY_REPLY:
	case MGMT_OP_USER_CONFIRM_NEG_REPLY:
		err = user_confirm_reply(sk, index, buf + sizeof(*hdr),
								len, opcode);
		break;
	case MGMT_OP_SET_LOCAL_NAME:
		err = set_local_name(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_START_DISCOVERY:
		err = start_discovery(sk, index);
		break;
	case MGMT_OP_STOP_DISCOVERY:
		err = stop_discovery(sk, index);
		break;
	case MGMT_OP_RESOLVE_NAME:
		err = resolve_name(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_CANCEL_RESOLVE_NAME:
		err = cancel_resolve_name(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_CONNECTION_PARAMS:
		err = set_connection_params(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_SET_RSSI_REPORTER:
		err = set_rssi_reporter(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_UNSET_RSSI_REPORTER:
		err = unset_rssi_reporter(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_READ_LOCAL_OOB_DATA:
		err = read_local_oob_data(sk, index);
		break;
	case MGMT_OP_ADD_REMOTE_OOB_DATA:
		err = add_remote_oob_data(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_REMOVE_REMOTE_OOB_DATA:
		err = remove_remote_oob_data(sk, index, buf + sizeof(*hdr),
									len);
		break;
	case MGMT_OP_ENCRYPT_LINK:
		err = encrypt_link(sk, index, buf + sizeof(*hdr), len);
		break;
	case MGMT_OP_LE_ADD_DEV_WHITE_LIST:
		err = le_add_dev_white_list(sk, index, buf + sizeof(*hdr),
									len);
		break;
	case MGMT_OP_LE_REMOVE_DEV_WHITE_LIST:
		err = le_remove_dev_white_list(sk, index, buf + sizeof(*hdr),
									len);
		break;
	case MGMT_OP_LE_CLEAR_WHITE_LIST:
		err = le_clear_white_list(sk, index);
		break;
	case MGMT_OP_LE_CREATE_CONN_WHITE_LIST:
		err = le_create_conn_white_list(sk, index);
		break;
	case MGMT_OP_LE_CANCEL_CREATE_CONN_WHITE_LIST:
		err = le_cancel_create_conn_white_list(sk, index);
		break;
	case MGMT_OP_LE_CANCEL_CREATE_CONN:
		err = le_cancel_create_conn(sk, index, buf + sizeof(*hdr), len);
		break;
	default:
		BT_DBG("Unknown op %u", opcode);
		err = cmd_status(sk, index, opcode, 0x01);
		break;
	}

	if (err < 0)
		goto done;
=======
		if (cp->val) {
			hdev->cur_adv_instance = 0x00;
			changed = !hci_dev_test_and_set_flag(hdev, HCI_ADVERTISING);
			if (cp->val == 0x02)
				hci_dev_set_flag(hdev, HCI_ADVERTISING_CONNECTABLE);
			else
				hci_dev_clear_flag(hdev, HCI_ADVERTISING_CONNECTABLE);
		} else {
			changed = hci_dev_test_and_clear_flag(hdev, HCI_ADVERTISING);
			hci_dev_clear_flag(hdev, HCI_ADVERTISING_CONNECTABLE);
		}

		err = send_settings_rsp(sk, MGMT_OP_SET_ADVERTISING, hdev);
		if (err < 0)
			goto unlock;

		if (changed)
			err = new_settings(hdev, sk);

		goto unlock;
	}

	if (pending_find(MGMT_OP_SET_ADVERTISING, hdev) ||
	    pending_find(MGMT_OP_SET_LE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_ADVERTISING,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_ADVERTISING, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	hci_req_init(&req, hdev);

	if (cp->val == 0x02)
		hci_dev_set_flag(hdev, HCI_ADVERTISING_CONNECTABLE);
	else
		hci_dev_clear_flag(hdev, HCI_ADVERTISING_CONNECTABLE);

	cancel_adv_timeout(hdev);
>>>>>>> android-4.9

	if (val) {
		/* Switch to instance "0" for the Set Advertising setting.
		 * We cannot use update_[adv|scan_rsp]_data() here as the
		 * HCI_ADVERTISING flag is not yet set.
		 */
		hdev->cur_adv_instance = 0x00;
		__hci_req_update_adv_data(&req, 0x00);
		__hci_req_update_scan_rsp_data(&req, 0x00);
		__hci_req_enable_advertising(&req);
	} else {
		__hci_req_disable_advertising(&req);
	}

<<<<<<< HEAD
done:
	kfree(buf);
=======
	err = hci_req_run(&req, set_advertising_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
>>>>>>> android-4.9
	return err;
}

static int set_static_address(struct sock *sk, struct hci_dev *hdev,
			      void *data, u16 len)
{
	struct mgmt_cp_set_static_address *cp = data;
	int err;

<<<<<<< HEAD
int mgmt_index_added(u16 index)
{
	BT_DBG("%d", index);
	return mgmt_event(MGMT_EV_INDEX_ADDED, index, NULL, 0, NULL);
}

int mgmt_index_removed(u16 index)
{
	u8 status = ENODEV;

	BT_DBG("%d", index);

	mgmt_pending_foreach(0, index, cmd_status_rsp, &status);

	return mgmt_event(MGMT_EV_INDEX_REMOVED, index, NULL, 0, NULL);
}

struct cmd_lookup {
	u8 val;
	struct sock *sk;
};

static void mode_rsp(struct pending_cmd *cmd, void *data)
{
	struct mgmt_mode *cp = cmd->param;
	struct cmd_lookup *match = data;

	if (cp->val != match->val)
		return;

	send_mode_rsp(cmd->sk, cmd->opcode, cmd->index, cp->val);
=======
	BT_DBG("%s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_STATIC_ADDRESS,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (hdev_is_powered(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_STATIC_ADDRESS,
				       MGMT_STATUS_REJECTED);

	if (bacmp(&cp->bdaddr, BDADDR_ANY)) {
		if (!bacmp(&cp->bdaddr, BDADDR_NONE))
			return mgmt_cmd_status(sk, hdev->id,
					       MGMT_OP_SET_STATIC_ADDRESS,
					       MGMT_STATUS_INVALID_PARAMS);

		/* Two most significant bits shall be set */
		if ((cp->bdaddr.b[5] & 0xc0) != 0xc0)
			return mgmt_cmd_status(sk, hdev->id,
					       MGMT_OP_SET_STATIC_ADDRESS,
					       MGMT_STATUS_INVALID_PARAMS);
	}

	hci_dev_lock(hdev);

	bacpy(&hdev->static_addr, &cp->bdaddr);
>>>>>>> android-4.9

	err = send_settings_rsp(sk, MGMT_OP_SET_STATIC_ADDRESS, hdev);
	if (err < 0)
		goto unlock;

	err = new_settings(hdev, sk);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

<<<<<<< HEAD
int mgmt_powered(u16 index, u8 powered)
{
	struct mgmt_mode ev;
	struct cmd_lookup match = { powered, NULL };
	int ret;

	BT_DBG("hci%u %d", index, powered);

	mgmt_pending_foreach(MGMT_OP_SET_POWERED, index, mode_rsp, &match);

	if (!powered) {
		u8 status = ENETDOWN;
		mgmt_pending_foreach(0, index, cmd_status_rsp, &status);
	}

	ev.val = powered;

	ret = mgmt_event(MGMT_EV_POWERED, index, &ev, sizeof(ev), match.sk);
=======
static int set_scan_params(struct sock *sk, struct hci_dev *hdev,
			   void *data, u16 len)
{
	struct mgmt_cp_set_scan_params *cp = data;
	__u16 interval, window;
	int err;

	BT_DBG("%s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SCAN_PARAMS,
				       MGMT_STATUS_NOT_SUPPORTED);

	interval = __le16_to_cpu(cp->interval);

	if (interval < 0x0004 || interval > 0x4000)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SCAN_PARAMS,
				       MGMT_STATUS_INVALID_PARAMS);

	window = __le16_to_cpu(cp->window);

	if (window < 0x0004 || window > 0x4000)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SCAN_PARAMS,
				       MGMT_STATUS_INVALID_PARAMS);

	if (window > interval)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SCAN_PARAMS,
				       MGMT_STATUS_INVALID_PARAMS);
>>>>>>> android-4.9

	hci_dev_lock(hdev);

<<<<<<< HEAD
	return ret;
}

int mgmt_discoverable(u16 index, u8 discoverable)
{
	struct mgmt_mode ev;
	struct cmd_lookup match = { discoverable, NULL };
	int ret;

	mgmt_pending_foreach(MGMT_OP_SET_DISCOVERABLE, index, mode_rsp, &match);

	ev.val = discoverable;

	ret = mgmt_event(MGMT_EV_DISCOVERABLE, index, &ev, sizeof(ev),
								match.sk);
=======
	hdev->le_scan_interval = interval;
	hdev->le_scan_window = window;

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_SET_SCAN_PARAMS, 0,
				NULL, 0);

	/* If background scan is running, restart it so new parameters are
	 * loaded.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_SCAN) &&
	    hdev->discovery.state == DISCOVERY_STOPPED) {
		struct hci_request req;

		hci_req_init(&req, hdev);

		hci_req_add_le_scan_disable(&req);
		hci_req_add_le_passive_scan(&req);
>>>>>>> android-4.9

		hci_req_run(&req, NULL);
	}

	hci_dev_unlock(hdev);

	return ret;
}

<<<<<<< HEAD
int mgmt_connectable(u16 index, u8 connectable)
{
	struct mgmt_mode ev;
	struct cmd_lookup match = { connectable, NULL };
	int ret;

	mgmt_pending_foreach(MGMT_OP_SET_CONNECTABLE, index, mode_rsp, &match);

	ev.val = connectable;

	ret = mgmt_event(MGMT_EV_CONNECTABLE, index, &ev, sizeof(ev), match.sk);
=======
static void fast_connectable_complete(struct hci_dev *hdev, u8 status,
				      u16 opcode)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status 0x%02x", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_FAST_CONNECTABLE, hdev);
	if (!cmd)
		goto unlock;

	if (status) {
		mgmt_cmd_status(cmd->sk, hdev->id, MGMT_OP_SET_FAST_CONNECTABLE,
			        mgmt_status(status));
	} else {
		struct mgmt_mode *cp = cmd->param;

		if (cp->val)
			hci_dev_set_flag(hdev, HCI_FAST_CONNECTABLE);
		else
			hci_dev_clear_flag(hdev, HCI_FAST_CONNECTABLE);

		send_settings_rsp(cmd->sk, MGMT_OP_SET_FAST_CONNECTABLE, hdev);
		new_settings(hdev, cmd->sk);
	}

	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int set_fast_connectable(struct sock *sk, struct hci_dev *hdev,
				void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;

	BT_DBG("%s", hdev->name);

	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) ||
	    hdev->hci_ver < BLUETOOTH_VER_1_2)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_FAST_CONNECTABLE,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_FAST_CONNECTABLE,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (pending_find(MGMT_OP_SET_FAST_CONNECTABLE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_FAST_CONNECTABLE,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if (!!cp->val == hci_dev_test_flag(hdev, HCI_FAST_CONNECTABLE)) {
		err = send_settings_rsp(sk, MGMT_OP_SET_FAST_CONNECTABLE,
					hdev);
		goto unlock;
	}

	if (!hdev_is_powered(hdev)) {
		hci_dev_change_flag(hdev, HCI_FAST_CONNECTABLE);
		err = send_settings_rsp(sk, MGMT_OP_SET_FAST_CONNECTABLE,
					hdev);
		new_settings(hdev, sk);
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_FAST_CONNECTABLE, hdev,
			       data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	hci_req_init(&req, hdev);

	__hci_req_write_fast_connectable(&req, cp->val);

	err = hci_req_run(&req, fast_connectable_complete);
	if (err < 0) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_FAST_CONNECTABLE,
				      MGMT_STATUS_FAILED);
		mgmt_pending_remove(cmd);
	}

unlock:
	hci_dev_unlock(hdev);

	return err;
}

static void set_bredr_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct mgmt_pending_cmd *cmd;

	BT_DBG("status 0x%02x", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_BREDR, hdev);
	if (!cmd)
		goto unlock;

	if (status) {
		u8 mgmt_err = mgmt_status(status);

		/* We need to restore the flag if related HCI commands
		 * failed.
		 */
		hci_dev_clear_flag(hdev, HCI_BREDR_ENABLED);

		mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode, mgmt_err);
	} else {
		send_settings_rsp(cmd->sk, MGMT_OP_SET_BREDR, hdev);
		new_settings(hdev, cmd->sk);
	}

	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int set_bredr(struct sock *sk, struct hci_dev *hdev, void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (!lmp_bredr_capable(hdev) || !lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
				       MGMT_STATUS_REJECTED);

	if (cp->val != 0x00 && cp->val != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (cp->val == hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		err = send_settings_rsp(sk, MGMT_OP_SET_BREDR, hdev);
		goto unlock;
	}

	if (!hdev_is_powered(hdev)) {
		if (!cp->val) {
			hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
			hci_dev_clear_flag(hdev, HCI_SSP_ENABLED);
			hci_dev_clear_flag(hdev, HCI_LINK_SECURITY);
			hci_dev_clear_flag(hdev, HCI_FAST_CONNECTABLE);
			hci_dev_clear_flag(hdev, HCI_HS_ENABLED);
		}

		hci_dev_change_flag(hdev, HCI_BREDR_ENABLED);

		err = send_settings_rsp(sk, MGMT_OP_SET_BREDR, hdev);
		if (err < 0)
			goto unlock;

		err = new_settings(hdev, sk);
		goto unlock;
	}

	/* Reject disabling when powered on */
	if (!cp->val) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
				      MGMT_STATUS_REJECTED);
		goto unlock;
	} else {
		/* When configuring a dual-mode controller to operate
		 * with LE only and using a static address, then switching
		 * BR/EDR back on is not allowed.
		 *
		 * Dual-mode controllers shall operate with the public
		 * address as its identity address for BR/EDR and LE. So
		 * reject the attempt to create an invalid configuration.
		 *
		 * The same restrictions applies when secure connections
		 * has been enabled. For BR/EDR this is a controller feature
		 * while for LE it is a host stack feature. This means that
		 * switching BR/EDR back on when secure connections has been
		 * enabled is not a supported transaction.
		 */
		if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) &&
		    (bacmp(&hdev->static_addr, BDADDR_ANY) ||
		     hci_dev_test_flag(hdev, HCI_SC_ENABLED))) {
			err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
					      MGMT_STATUS_REJECTED);
			goto unlock;
		}
	}

	if (pending_find(MGMT_OP_SET_BREDR, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_BREDR,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_BREDR, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	/* We need to flip the bit already here so that
	 * hci_req_update_adv_data generates the correct flags.
	 */
	hci_dev_set_flag(hdev, HCI_BREDR_ENABLED);

	hci_req_init(&req, hdev);

	__hci_req_write_fast_connectable(&req, false);
	__hci_req_update_scan(&req);

	/* Since only the advertising data flags will change, there
	 * is no need to update the scan response data.
	 */
	__hci_req_update_adv_data(&req, hdev->cur_adv_instance);

	err = hci_req_run(&req, set_bredr_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static void sc_enable_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct mgmt_pending_cmd *cmd;
	struct mgmt_mode *cp;

	BT_DBG("%s status %u", hdev->name, status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_SET_SECURE_CONN, hdev);
	if (!cmd)
		goto unlock;

	if (status) {
		mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode,
			        mgmt_status(status));
		goto remove;
	}

	cp = cmd->param;

	switch (cp->val) {
	case 0x00:
		hci_dev_clear_flag(hdev, HCI_SC_ENABLED);
		hci_dev_clear_flag(hdev, HCI_SC_ONLY);
		break;
	case 0x01:
		hci_dev_set_flag(hdev, HCI_SC_ENABLED);
		hci_dev_clear_flag(hdev, HCI_SC_ONLY);
		break;
	case 0x02:
		hci_dev_set_flag(hdev, HCI_SC_ENABLED);
		hci_dev_set_flag(hdev, HCI_SC_ONLY);
		break;
	}

	send_settings_rsp(cmd->sk, MGMT_OP_SET_SECURE_CONN, hdev);
	new_settings(hdev, cmd->sk);

remove:
	mgmt_pending_remove(cmd);
unlock:
	hci_dev_unlock(hdev);
}

static int set_secure_conn(struct sock *sk, struct hci_dev *hdev,
			   void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	u8 val;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (!lmp_sc_capable(hdev) &&
	    !hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SECURE_CONN,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) &&
	    lmp_sc_capable(hdev) &&
	    !hci_dev_test_flag(hdev, HCI_SSP_ENABLED))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SECURE_CONN,
				       MGMT_STATUS_REJECTED);

	if (cp->val != 0x00 && cp->val != 0x01 && cp->val != 0x02)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SECURE_CONN,
				  MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev) || !lmp_sc_capable(hdev) ||
	    !hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		bool changed;

		if (cp->val) {
			changed = !hci_dev_test_and_set_flag(hdev,
							     HCI_SC_ENABLED);
			if (cp->val == 0x02)
				hci_dev_set_flag(hdev, HCI_SC_ONLY);
			else
				hci_dev_clear_flag(hdev, HCI_SC_ONLY);
		} else {
			changed = hci_dev_test_and_clear_flag(hdev,
							      HCI_SC_ENABLED);
			hci_dev_clear_flag(hdev, HCI_SC_ONLY);
		}

		err = send_settings_rsp(sk, MGMT_OP_SET_SECURE_CONN, hdev);
		if (err < 0)
			goto failed;

		if (changed)
			err = new_settings(hdev, sk);

		goto failed;
	}

	if (pending_find(MGMT_OP_SET_SECURE_CONN, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_SECURE_CONN,
				      MGMT_STATUS_BUSY);
		goto failed;
	}

	val = !!cp->val;

	if (val == hci_dev_test_flag(hdev, HCI_SC_ENABLED) &&
	    (cp->val == 0x02) == hci_dev_test_flag(hdev, HCI_SC_ONLY)) {
		err = send_settings_rsp(sk, MGMT_OP_SET_SECURE_CONN, hdev);
		goto failed;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_SET_SECURE_CONN, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto failed;
	}

	hci_req_init(&req, hdev);
	hci_req_add(&req, HCI_OP_WRITE_SC_SUPPORT, 1, &val);
	err = hci_req_run(&req, sc_enable_complete);
	if (err < 0) {
		mgmt_pending_remove(cmd);
		goto failed;
	}

failed:
	hci_dev_unlock(hdev);
	return err;
}

static int set_debug_keys(struct sock *sk, struct hci_dev *hdev,
			  void *data, u16 len)
{
	struct mgmt_mode *cp = data;
	bool changed, use_changed;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (cp->val != 0x00 && cp->val != 0x01 && cp->val != 0x02)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_DEBUG_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (cp->val)
		changed = !hci_dev_test_and_set_flag(hdev, HCI_KEEP_DEBUG_KEYS);
	else
		changed = hci_dev_test_and_clear_flag(hdev,
						      HCI_KEEP_DEBUG_KEYS);

	if (cp->val == 0x02)
		use_changed = !hci_dev_test_and_set_flag(hdev,
							 HCI_USE_DEBUG_KEYS);
	else
		use_changed = hci_dev_test_and_clear_flag(hdev,
							  HCI_USE_DEBUG_KEYS);

	if (hdev_is_powered(hdev) && use_changed &&
	    hci_dev_test_flag(hdev, HCI_SSP_ENABLED)) {
		u8 mode = (cp->val == 0x02) ? 0x01 : 0x00;
		hci_send_cmd(hdev, HCI_OP_WRITE_SSP_DEBUG_MODE,
			     sizeof(mode), &mode);
	}

	err = send_settings_rsp(sk, MGMT_OP_SET_DEBUG_KEYS, hdev);
	if (err < 0)
		goto unlock;

	if (changed)
		err = new_settings(hdev, sk);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int set_privacy(struct sock *sk, struct hci_dev *hdev, void *cp_data,
		       u16 len)
{
	struct mgmt_cp_set_privacy *cp = cp_data;
	bool changed;
	int err;

	BT_DBG("request for %s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PRIVACY,
				       MGMT_STATUS_NOT_SUPPORTED);

	if (cp->privacy != 0x00 && cp->privacy != 0x01 && cp->privacy != 0x02)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PRIVACY,
				       MGMT_STATUS_INVALID_PARAMS);

	if (hdev_is_powered(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PRIVACY,
				       MGMT_STATUS_REJECTED);

	hci_dev_lock(hdev);

	/* If user space supports this command it is also expected to
	 * handle IRKs. Therefore, set the HCI_RPA_RESOLVING flag.
	 */
	hci_dev_set_flag(hdev, HCI_RPA_RESOLVING);

	if (cp->privacy) {
		changed = !hci_dev_test_and_set_flag(hdev, HCI_PRIVACY);
		memcpy(hdev->irk, cp->irk, sizeof(hdev->irk));
		hci_dev_set_flag(hdev, HCI_RPA_EXPIRED);
		if (cp->privacy == 0x02)
			hci_dev_set_flag(hdev, HCI_LIMITED_PRIVACY);
		else
			hci_dev_clear_flag(hdev, HCI_LIMITED_PRIVACY);
	} else {
		changed = hci_dev_test_and_clear_flag(hdev, HCI_PRIVACY);
		memset(hdev->irk, 0, sizeof(hdev->irk));
		hci_dev_clear_flag(hdev, HCI_RPA_EXPIRED);
		hci_dev_clear_flag(hdev, HCI_LIMITED_PRIVACY);
	}

	err = send_settings_rsp(sk, MGMT_OP_SET_PRIVACY, hdev);
	if (err < 0)
		goto unlock;

	if (changed)
		err = new_settings(hdev, sk);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static bool irk_is_valid(struct mgmt_irk_info *irk)
{
	switch (irk->addr.type) {
	case BDADDR_LE_PUBLIC:
		return true;

	case BDADDR_LE_RANDOM:
		/* Two most significant bits shall be set */
		if ((irk->addr.bdaddr.b[5] & 0xc0) != 0xc0)
			return false;
		return true;
	}

	return false;
}

static int load_irks(struct sock *sk, struct hci_dev *hdev, void *cp_data,
		     u16 len)
{
	struct mgmt_cp_load_irks *cp = cp_data;
	const u16 max_irk_count = ((U16_MAX - sizeof(*cp)) /
				   sizeof(struct mgmt_irk_info));
	u16 irk_count, expected_len;
	int i, err;

	BT_DBG("request for %s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_IRKS,
				       MGMT_STATUS_NOT_SUPPORTED);

	irk_count = __le16_to_cpu(cp->irk_count);
	if (irk_count > max_irk_count) {
		BT_ERR("load_irks: too big irk_count value %u", irk_count);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_IRKS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	expected_len = sizeof(*cp) + irk_count * sizeof(struct mgmt_irk_info);
	if (expected_len != len) {
		BT_ERR("load_irks: expected %u bytes, got %u bytes",
		       expected_len, len);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_IRKS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	BT_DBG("%s irk_count %u", hdev->name, irk_count);

	for (i = 0; i < irk_count; i++) {
		struct mgmt_irk_info *key = &cp->irks[i];

		if (!irk_is_valid(key))
			return mgmt_cmd_status(sk, hdev->id,
					       MGMT_OP_LOAD_IRKS,
					       MGMT_STATUS_INVALID_PARAMS);
	}

	hci_dev_lock(hdev);

	hci_smp_irks_clear(hdev);

	for (i = 0; i < irk_count; i++) {
		struct mgmt_irk_info *irk = &cp->irks[i];

		hci_add_irk(hdev, &irk->addr.bdaddr,
			    le_addr_type(irk->addr.type), irk->val,
			    BDADDR_ANY);
	}

	hci_dev_set_flag(hdev, HCI_RPA_RESOLVING);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_LOAD_IRKS, 0, NULL, 0);

	hci_dev_unlock(hdev);

	return err;
}

static bool ltk_is_valid(struct mgmt_ltk_info *key)
{
	if (key->master != 0x00 && key->master != 0x01)
		return false;

	switch (key->addr.type) {
	case BDADDR_LE_PUBLIC:
		return true;

	case BDADDR_LE_RANDOM:
		/* Two most significant bits shall be set */
		if ((key->addr.bdaddr.b[5] & 0xc0) != 0xc0)
			return false;
		return true;
	}

	return false;
}

static int load_long_term_keys(struct sock *sk, struct hci_dev *hdev,
			       void *cp_data, u16 len)
{
	struct mgmt_cp_load_long_term_keys *cp = cp_data;
	const u16 max_key_count = ((U16_MAX - sizeof(*cp)) /
				   sizeof(struct mgmt_ltk_info));
	u16 key_count, expected_len;
	int i, err;

	BT_DBG("request for %s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LONG_TERM_KEYS,
				       MGMT_STATUS_NOT_SUPPORTED);

	key_count = __le16_to_cpu(cp->key_count);
	if (key_count > max_key_count) {
		BT_ERR("load_ltks: too big key_count value %u", key_count);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LONG_TERM_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	expected_len = sizeof(*cp) + key_count *
					sizeof(struct mgmt_ltk_info);
	if (expected_len != len) {
		BT_ERR("load_keys: expected %u bytes, got %u bytes",
		       expected_len, len);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_LONG_TERM_KEYS,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	BT_DBG("%s key_count %u", hdev->name, key_count);

	for (i = 0; i < key_count; i++) {
		struct mgmt_ltk_info *key = &cp->keys[i];

		if (!ltk_is_valid(key))
			return mgmt_cmd_status(sk, hdev->id,
					       MGMT_OP_LOAD_LONG_TERM_KEYS,
					       MGMT_STATUS_INVALID_PARAMS);
	}

	hci_dev_lock(hdev);

	hci_smp_ltks_clear(hdev);

	for (i = 0; i < key_count; i++) {
		struct mgmt_ltk_info *key = &cp->keys[i];
		u8 type, authenticated;

		switch (key->type) {
		case MGMT_LTK_UNAUTHENTICATED:
			authenticated = 0x00;
			type = key->master ? SMP_LTK : SMP_LTK_SLAVE;
			break;
		case MGMT_LTK_AUTHENTICATED:
			authenticated = 0x01;
			type = key->master ? SMP_LTK : SMP_LTK_SLAVE;
			break;
		case MGMT_LTK_P256_UNAUTH:
			authenticated = 0x00;
			type = SMP_LTK_P256;
			break;
		case MGMT_LTK_P256_AUTH:
			authenticated = 0x01;
			type = SMP_LTK_P256;
			break;
		case MGMT_LTK_P256_DEBUG:
			authenticated = 0x00;
			type = SMP_LTK_P256_DEBUG;
		default:
			continue;
		}

		hci_add_ltk(hdev, &key->addr.bdaddr,
			    le_addr_type(key->addr.type), type, authenticated,
			    key->val, key->enc_size, key->ediv, key->rand);
	}

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_LOAD_LONG_TERM_KEYS, 0,
			   NULL, 0);

	hci_dev_unlock(hdev);

	return err;
}

static int conn_info_cmd_complete(struct mgmt_pending_cmd *cmd, u8 status)
{
	struct hci_conn *conn = cmd->user_data;
	struct mgmt_rp_get_conn_info rp;
	int err;

	memcpy(&rp.addr, cmd->param, sizeof(rp.addr));

	if (status == MGMT_STATUS_SUCCESS) {
		rp.rssi = conn->rssi;
		rp.tx_power = conn->tx_power;
		rp.max_tx_power = conn->max_tx_power;
	} else {
		rp.rssi = HCI_RSSI_INVALID;
		rp.tx_power = HCI_TX_POWER_INVALID;
		rp.max_tx_power = HCI_TX_POWER_INVALID;
	}

	err = mgmt_cmd_complete(cmd->sk, cmd->index, MGMT_OP_GET_CONN_INFO,
				status, &rp, sizeof(rp));

	hci_conn_drop(conn);
	hci_conn_put(conn);

	return err;
}

static void conn_info_refresh_complete(struct hci_dev *hdev, u8 hci_status,
				       u16 opcode)
{
	struct hci_cp_read_rssi *cp;
	struct mgmt_pending_cmd *cmd;
	struct hci_conn *conn;
	u16 handle;
	u8 status;

	BT_DBG("status 0x%02x", hci_status);

	hci_dev_lock(hdev);

	/* Commands sent in request are either Read RSSI or Read Transmit Power
	 * Level so we check which one was last sent to retrieve connection
	 * handle.  Both commands have handle as first parameter so it's safe to
	 * cast data on the same command struct.
	 *
	 * First command sent is always Read RSSI and we fail only if it fails.
	 * In other case we simply override error to indicate success as we
	 * already remembered if TX power value is actually valid.
	 */
	cp = hci_sent_cmd_data(hdev, HCI_OP_READ_RSSI);
	if (!cp) {
		cp = hci_sent_cmd_data(hdev, HCI_OP_READ_TX_POWER);
		status = MGMT_STATUS_SUCCESS;
	} else {
		status = mgmt_status(hci_status);
	}

	if (!cp) {
		BT_ERR("invalid sent_cmd in conn_info response");
		goto unlock;
	}

	handle = __le16_to_cpu(cp->handle);
	conn = hci_conn_hash_lookup_handle(hdev, handle);
	if (!conn) {
		BT_ERR("unknown handle (%d) in conn_info response", handle);
		goto unlock;
	}

	cmd = pending_find_data(MGMT_OP_GET_CONN_INFO, hdev, conn);
	if (!cmd)
		goto unlock;

	cmd->cmd_complete(cmd, status);
	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int get_conn_info(struct sock *sk, struct hci_dev *hdev, void *data,
			 u16 len)
{
	struct mgmt_cp_get_conn_info *cp = data;
	struct mgmt_rp_get_conn_info rp;
	struct hci_conn *conn;
	unsigned long conn_info_age;
	int err = 0;

	BT_DBG("%s", hdev->name);

	memset(&rp, 0, sizeof(rp));
	bacpy(&rp.addr.bdaddr, &cp->addr.bdaddr);
	rp.addr.type = cp->addr.type;

	if (!bdaddr_type_is_valid(cp->addr.type))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONN_INFO,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONN_INFO,
					MGMT_STATUS_NOT_POWERED, &rp,
					sizeof(rp));
		goto unlock;
	}

	if (cp->addr.type == BDADDR_BREDR)
		conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK,
					       &cp->addr.bdaddr);
	else
		conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, &cp->addr.bdaddr);

	if (!conn || conn->state != BT_CONNECTED) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONN_INFO,
					MGMT_STATUS_NOT_CONNECTED, &rp,
					sizeof(rp));
		goto unlock;
	}

	if (pending_find_data(MGMT_OP_GET_CONN_INFO, hdev, conn)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONN_INFO,
					MGMT_STATUS_BUSY, &rp, sizeof(rp));
		goto unlock;
	}

	/* To avoid client trying to guess when to poll again for information we
	 * calculate conn info age as random value between min/max set in hdev.
	 */
	conn_info_age = hdev->conn_info_min_age +
			prandom_u32_max(hdev->conn_info_max_age -
					hdev->conn_info_min_age);

	/* Query controller to refresh cached values if they are too old or were
	 * never read.
	 */
	if (time_after(jiffies, conn->conn_info_timestamp +
		       msecs_to_jiffies(conn_info_age)) ||
	    !conn->conn_info_timestamp) {
		struct hci_request req;
		struct hci_cp_read_tx_power req_txp_cp;
		struct hci_cp_read_rssi req_rssi_cp;
		struct mgmt_pending_cmd *cmd;

		hci_req_init(&req, hdev);
		req_rssi_cp.handle = cpu_to_le16(conn->handle);
		hci_req_add(&req, HCI_OP_READ_RSSI, sizeof(req_rssi_cp),
			    &req_rssi_cp);

		/* For LE links TX power does not change thus we don't need to
		 * query for it once value is known.
		 */
		if (!bdaddr_type_is_le(cp->addr.type) ||
		    conn->tx_power == HCI_TX_POWER_INVALID) {
			req_txp_cp.handle = cpu_to_le16(conn->handle);
			req_txp_cp.type = 0x00;
			hci_req_add(&req, HCI_OP_READ_TX_POWER,
				    sizeof(req_txp_cp), &req_txp_cp);
		}

		/* Max TX power needs to be read only once per connection */
		if (conn->max_tx_power == HCI_TX_POWER_INVALID) {
			req_txp_cp.handle = cpu_to_le16(conn->handle);
			req_txp_cp.type = 0x01;
			hci_req_add(&req, HCI_OP_READ_TX_POWER,
				    sizeof(req_txp_cp), &req_txp_cp);
		}

		err = hci_req_run(&req, conn_info_refresh_complete);
		if (err < 0)
			goto unlock;

		cmd = mgmt_pending_add(sk, MGMT_OP_GET_CONN_INFO, hdev,
				       data, len);
		if (!cmd) {
			err = -ENOMEM;
			goto unlock;
		}

		hci_conn_hold(conn);
		cmd->user_data = hci_conn_get(conn);
		cmd->cmd_complete = conn_info_cmd_complete;

		conn->conn_info_timestamp = jiffies;
	} else {
		/* Cache is valid, just reply with values cached in hci_conn */
		rp.rssi = conn->rssi;
		rp.tx_power = conn->tx_power;
		rp.max_tx_power = conn->max_tx_power;

		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CONN_INFO,
					MGMT_STATUS_SUCCESS, &rp, sizeof(rp));
	}

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int clock_info_cmd_complete(struct mgmt_pending_cmd *cmd, u8 status)
{
	struct hci_conn *conn = cmd->user_data;
	struct mgmt_rp_get_clock_info rp;
	struct hci_dev *hdev;
	int err;

	memset(&rp, 0, sizeof(rp));
	memcpy(&rp.addr, cmd->param, sizeof(rp.addr));

	if (status)
		goto complete;

	hdev = hci_dev_get(cmd->index);
	if (hdev) {
		rp.local_clock = cpu_to_le32(hdev->clock);
		hci_dev_put(hdev);
	}

	if (conn) {
		rp.piconet_clock = cpu_to_le32(conn->clock);
		rp.accuracy = cpu_to_le16(conn->clock_accuracy);
	}

complete:
	err = mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode, status, &rp,
				sizeof(rp));

	if (conn) {
		hci_conn_drop(conn);
		hci_conn_put(conn);
	}

	return err;
}

static void get_clock_info_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	struct hci_cp_read_clock *hci_cp;
	struct mgmt_pending_cmd *cmd;
	struct hci_conn *conn;

	BT_DBG("%s status %u", hdev->name, status);

	hci_dev_lock(hdev);

	hci_cp = hci_sent_cmd_data(hdev, HCI_OP_READ_CLOCK);
	if (!hci_cp)
		goto unlock;

	if (hci_cp->which) {
		u16 handle = __le16_to_cpu(hci_cp->handle);
		conn = hci_conn_hash_lookup_handle(hdev, handle);
	} else {
		conn = NULL;
	}

	cmd = pending_find_data(MGMT_OP_GET_CLOCK_INFO, hdev, conn);
	if (!cmd)
		goto unlock;

	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int get_clock_info(struct sock *sk, struct hci_dev *hdev, void *data,
			 u16 len)
{
	struct mgmt_cp_get_clock_info *cp = data;
	struct mgmt_rp_get_clock_info rp;
	struct hci_cp_read_clock hci_cp;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	struct hci_conn *conn;
	int err;

	BT_DBG("%s", hdev->name);

	memset(&rp, 0, sizeof(rp));
	bacpy(&rp.addr.bdaddr, &cp->addr.bdaddr);
	rp.addr.type = cp->addr.type;

	if (cp->addr.type != BDADDR_BREDR)
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CLOCK_INFO,
					 MGMT_STATUS_INVALID_PARAMS,
					 &rp, sizeof(rp));

	hci_dev_lock(hdev);

	if (!hdev_is_powered(hdev)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_CLOCK_INFO,
					MGMT_STATUS_NOT_POWERED, &rp,
					sizeof(rp));
		goto unlock;
	}

	if (bacmp(&cp->addr.bdaddr, BDADDR_ANY)) {
		conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK,
					       &cp->addr.bdaddr);
		if (!conn || conn->state != BT_CONNECTED) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_GET_CLOCK_INFO,
						MGMT_STATUS_NOT_CONNECTED,
						&rp, sizeof(rp));
			goto unlock;
		}
	} else {
		conn = NULL;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_GET_CLOCK_INFO, hdev, data, len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	cmd->cmd_complete = clock_info_cmd_complete;

	hci_req_init(&req, hdev);

	memset(&hci_cp, 0, sizeof(hci_cp));
	hci_req_add(&req, HCI_OP_READ_CLOCK, sizeof(hci_cp), &hci_cp);

	if (conn) {
		hci_conn_hold(conn);
		cmd->user_data = hci_conn_get(conn);

		hci_cp.handle = cpu_to_le16(conn->handle);
		hci_cp.which = 0x01; /* Piconet clock */
		hci_req_add(&req, HCI_OP_READ_CLOCK, sizeof(hci_cp), &hci_cp);
	}

	err = hci_req_run(&req, get_clock_info_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static bool is_connected(struct hci_dev *hdev, bdaddr_t *addr, u8 type)
{
	struct hci_conn *conn;

	conn = hci_conn_hash_lookup_ba(hdev, LE_LINK, addr);
	if (!conn)
		return false;

	if (conn->dst_type != type)
		return false;

	if (conn->state != BT_CONNECTED)
		return false;

	return true;
}

/* This function requires the caller holds hdev->lock */
static int hci_conn_params_set(struct hci_dev *hdev, bdaddr_t *addr,
			       u8 addr_type, u8 auto_connect)
{
	struct hci_conn_params *params;

	params = hci_conn_params_add(hdev, addr, addr_type);
	if (!params)
		return -EIO;

	if (params->auto_connect == auto_connect)
		return 0;

	list_del_init(&params->action);

	switch (auto_connect) {
	case HCI_AUTO_CONN_DISABLED:
	case HCI_AUTO_CONN_LINK_LOSS:
		/* If auto connect is being disabled when we're trying to
		 * connect to device, keep connecting.
		 */
		if (params->explicit_connect)
			list_add(&params->action, &hdev->pend_le_conns);
		break;
	case HCI_AUTO_CONN_REPORT:
		if (params->explicit_connect)
			list_add(&params->action, &hdev->pend_le_conns);
		else
			list_add(&params->action, &hdev->pend_le_reports);
		break;
	case HCI_AUTO_CONN_DIRECT:
	case HCI_AUTO_CONN_ALWAYS:
		if (!is_connected(hdev, addr, addr_type))
			list_add(&params->action, &hdev->pend_le_conns);
		break;
	}

	params->auto_connect = auto_connect;

	BT_DBG("addr %pMR (type %u) auto_connect %u", addr, addr_type,
	       auto_connect);

	return 0;
}

static void device_added(struct sock *sk, struct hci_dev *hdev,
			 bdaddr_t *bdaddr, u8 type, u8 action)
{
	struct mgmt_ev_device_added ev;

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = type;
	ev.action = action;

	mgmt_event(MGMT_EV_DEVICE_ADDED, hdev, &ev, sizeof(ev), sk);
}

static int add_device(struct sock *sk, struct hci_dev *hdev,
		      void *data, u16 len)
{
	struct mgmt_cp_add_device *cp = data;
	u8 auto_conn, addr_type;
	int err;

	BT_DBG("%s", hdev->name);

	if (!bdaddr_type_is_valid(cp->addr.type) ||
	    !bacmp(&cp->addr.bdaddr, BDADDR_ANY))
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &cp->addr, sizeof(cp->addr));

	if (cp->action != 0x00 && cp->action != 0x01 && cp->action != 0x02)
		return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_DEVICE,
					 MGMT_STATUS_INVALID_PARAMS,
					 &cp->addr, sizeof(cp->addr));

	hci_dev_lock(hdev);

	if (cp->addr.type == BDADDR_BREDR) {
		/* Only incoming connections action is supported for now */
		if (cp->action != 0x01) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_ADD_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		err = hci_bdaddr_list_add(&hdev->whitelist, &cp->addr.bdaddr,
					  cp->addr.type);
		if (err)
			goto unlock;

		hci_req_update_scan(hdev);

		goto added;
	}

	addr_type = le_addr_type(cp->addr.type);

	if (cp->action == 0x02)
		auto_conn = HCI_AUTO_CONN_ALWAYS;
	else if (cp->action == 0x01)
		auto_conn = HCI_AUTO_CONN_DIRECT;
	else
		auto_conn = HCI_AUTO_CONN_REPORT;

	/* Kernel internally uses conn_params with resolvable private
	 * address, but Add Device allows only identity addresses.
	 * Make sure it is enforced before calling
	 * hci_conn_params_lookup.
	 */
	if (!hci_is_identity_address(&cp->addr.bdaddr, addr_type)) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_DEVICE,
					MGMT_STATUS_INVALID_PARAMS,
					&cp->addr, sizeof(cp->addr));
		goto unlock;
	}

	/* If the connection parameters don't exist for this device,
	 * they will be created and configured with defaults.
	 */
	if (hci_conn_params_set(hdev, &cp->addr.bdaddr, addr_type,
				auto_conn) < 0) {
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_DEVICE,
					MGMT_STATUS_FAILED, &cp->addr,
					sizeof(cp->addr));
		goto unlock;
	}

	hci_update_background_scan(hdev);

added:
	device_added(sk, hdev, &cp->addr.bdaddr, cp->addr.type, cp->action);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_DEVICE,
				MGMT_STATUS_SUCCESS, &cp->addr,
				sizeof(cp->addr));

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static void device_removed(struct sock *sk, struct hci_dev *hdev,
			   bdaddr_t *bdaddr, u8 type)
{
	struct mgmt_ev_device_removed ev;

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = type;

	mgmt_event(MGMT_EV_DEVICE_REMOVED, hdev, &ev, sizeof(ev), sk);
}

static int remove_device(struct sock *sk, struct hci_dev *hdev,
			 void *data, u16 len)
{
	struct mgmt_cp_remove_device *cp = data;
	int err;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (bacmp(&cp->addr.bdaddr, BDADDR_ANY)) {
		struct hci_conn_params *params;
		u8 addr_type;

		if (!bdaddr_type_is_valid(cp->addr.type)) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		if (cp->addr.type == BDADDR_BREDR) {
			err = hci_bdaddr_list_del(&hdev->whitelist,
						  &cp->addr.bdaddr,
						  cp->addr.type);
			if (err) {
				err = mgmt_cmd_complete(sk, hdev->id,
							MGMT_OP_REMOVE_DEVICE,
							MGMT_STATUS_INVALID_PARAMS,
							&cp->addr,
							sizeof(cp->addr));
				goto unlock;
			}

			hci_req_update_scan(hdev);

			device_removed(sk, hdev, &cp->addr.bdaddr,
				       cp->addr.type);
			goto complete;
		}

		addr_type = le_addr_type(cp->addr.type);

		/* Kernel internally uses conn_params with resolvable private
		 * address, but Remove Device allows only identity addresses.
		 * Make sure it is enforced before calling
		 * hci_conn_params_lookup.
		 */
		if (!hci_is_identity_address(&cp->addr.bdaddr, addr_type)) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		params = hci_conn_params_lookup(hdev, &cp->addr.bdaddr,
						addr_type);
		if (!params) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		if (params->auto_connect == HCI_AUTO_CONN_DISABLED ||
		    params->auto_connect == HCI_AUTO_CONN_EXPLICIT) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		list_del(&params->action);
		list_del(&params->list);
		kfree(params);
		hci_update_background_scan(hdev);

		device_removed(sk, hdev, &cp->addr.bdaddr, cp->addr.type);
	} else {
		struct hci_conn_params *p, *tmp;
		struct bdaddr_list *b, *btmp;

		if (cp->addr.type) {
			err = mgmt_cmd_complete(sk, hdev->id,
						MGMT_OP_REMOVE_DEVICE,
						MGMT_STATUS_INVALID_PARAMS,
						&cp->addr, sizeof(cp->addr));
			goto unlock;
		}

		list_for_each_entry_safe(b, btmp, &hdev->whitelist, list) {
			device_removed(sk, hdev, &b->bdaddr, b->bdaddr_type);
			list_del(&b->list);
			kfree(b);
		}

		hci_req_update_scan(hdev);

		list_for_each_entry_safe(p, tmp, &hdev->le_conn_params, list) {
			if (p->auto_connect == HCI_AUTO_CONN_DISABLED)
				continue;
			device_removed(sk, hdev, &p->addr, p->addr_type);
			if (p->explicit_connect) {
				p->auto_connect = HCI_AUTO_CONN_EXPLICIT;
				continue;
			}
			list_del(&p->action);
			list_del(&p->list);
			kfree(p);
		}

		BT_DBG("All LE connection parameters were removed");

		hci_update_background_scan(hdev);
	}

complete:
	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_REMOVE_DEVICE,
				MGMT_STATUS_SUCCESS, &cp->addr,
				sizeof(cp->addr));
unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int load_conn_param(struct sock *sk, struct hci_dev *hdev, void *data,
			   u16 len)
{
	struct mgmt_cp_load_conn_param *cp = data;
	const u16 max_param_count = ((U16_MAX - sizeof(*cp)) /
				     sizeof(struct mgmt_conn_param));
	u16 param_count, expected_len;
	int i;

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_CONN_PARAM,
				       MGMT_STATUS_NOT_SUPPORTED);

	param_count = __le16_to_cpu(cp->param_count);
	if (param_count > max_param_count) {
		BT_ERR("load_conn_param: too big param_count value %u",
		       param_count);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_CONN_PARAM,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	expected_len = sizeof(*cp) + param_count *
					sizeof(struct mgmt_conn_param);
	if (expected_len != len) {
		BT_ERR("load_conn_param: expected %u bytes, got %u bytes",
		       expected_len, len);
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_LOAD_CONN_PARAM,
				       MGMT_STATUS_INVALID_PARAMS);
	}

	BT_DBG("%s param_count %u", hdev->name, param_count);

	hci_dev_lock(hdev);

	hci_conn_params_clear_disabled(hdev);

	for (i = 0; i < param_count; i++) {
		struct mgmt_conn_param *param = &cp->params[i];
		struct hci_conn_params *hci_param;
		u16 min, max, latency, timeout;
		u8 addr_type;

		BT_DBG("Adding %pMR (type %u)", &param->addr.bdaddr,
		       param->addr.type);

		if (param->addr.type == BDADDR_LE_PUBLIC) {
			addr_type = ADDR_LE_DEV_PUBLIC;
		} else if (param->addr.type == BDADDR_LE_RANDOM) {
			addr_type = ADDR_LE_DEV_RANDOM;
		} else {
			BT_ERR("Ignoring invalid connection parameters");
			continue;
		}

		min = le16_to_cpu(param->min_interval);
		max = le16_to_cpu(param->max_interval);
		latency = le16_to_cpu(param->latency);
		timeout = le16_to_cpu(param->timeout);

		BT_DBG("min 0x%04x max 0x%04x latency 0x%04x timeout 0x%04x",
		       min, max, latency, timeout);

		if (hci_check_conn_params(min, max, latency, timeout) < 0) {
			BT_ERR("Ignoring invalid connection parameters");
			continue;
		}

		hci_param = hci_conn_params_add(hdev, &param->addr.bdaddr,
						addr_type);
		if (!hci_param) {
			BT_ERR("Failed to add connection parameters");
			continue;
		}

		hci_param->conn_min_interval = min;
		hci_param->conn_max_interval = max;
		hci_param->conn_latency = latency;
		hci_param->supervision_timeout = timeout;
	}

	hci_dev_unlock(hdev);

	return mgmt_cmd_complete(sk, hdev->id, MGMT_OP_LOAD_CONN_PARAM, 0,
				 NULL, 0);
}

static int set_external_config(struct sock *sk, struct hci_dev *hdev,
			       void *data, u16 len)
{
	struct mgmt_cp_set_external_config *cp = data;
	bool changed;
	int err;

	BT_DBG("%s", hdev->name);

	if (hdev_is_powered(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_EXTERNAL_CONFIG,
				       MGMT_STATUS_REJECTED);

	if (cp->config != 0x00 && cp->config != 0x01)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_EXTERNAL_CONFIG,
				         MGMT_STATUS_INVALID_PARAMS);

	if (!test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_EXTERNAL_CONFIG,
				       MGMT_STATUS_NOT_SUPPORTED);

	hci_dev_lock(hdev);

	if (cp->config)
		changed = !hci_dev_test_and_set_flag(hdev, HCI_EXT_CONFIGURED);
	else
		changed = hci_dev_test_and_clear_flag(hdev, HCI_EXT_CONFIGURED);

	err = send_options_rsp(sk, MGMT_OP_SET_EXTERNAL_CONFIG, hdev);
	if (err < 0)
		goto unlock;

	if (!changed)
		goto unlock;

	err = new_options(hdev, sk);

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED) == is_configured(hdev)) {
		mgmt_index_removed(hdev);

		if (hci_dev_test_and_change_flag(hdev, HCI_UNCONFIGURED)) {
			hci_dev_set_flag(hdev, HCI_CONFIG);
			hci_dev_set_flag(hdev, HCI_AUTO_OFF);

			queue_work(hdev->req_workqueue, &hdev->power_on);
		} else {
			set_bit(HCI_RAW, &hdev->flags);
			mgmt_index_added(hdev);
		}
	}

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static int set_public_address(struct sock *sk, struct hci_dev *hdev,
			      void *data, u16 len)
{
	struct mgmt_cp_set_public_address *cp = data;
	bool changed;
	int err;

	BT_DBG("%s", hdev->name);

	if (hdev_is_powered(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PUBLIC_ADDRESS,
				       MGMT_STATUS_REJECTED);

	if (!bacmp(&cp->bdaddr, BDADDR_ANY))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PUBLIC_ADDRESS,
				       MGMT_STATUS_INVALID_PARAMS);

	if (!hdev->set_bdaddr)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_SET_PUBLIC_ADDRESS,
				       MGMT_STATUS_NOT_SUPPORTED);

	hci_dev_lock(hdev);

	changed = !!bacmp(&hdev->public_addr, &cp->bdaddr);
	bacpy(&hdev->public_addr, &cp->bdaddr);

	err = send_options_rsp(sk, MGMT_OP_SET_PUBLIC_ADDRESS, hdev);
	if (err < 0)
		goto unlock;

	if (!changed)
		goto unlock;

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED))
		err = new_options(hdev, sk);

	if (is_configured(hdev)) {
		mgmt_index_removed(hdev);

		hci_dev_clear_flag(hdev, HCI_UNCONFIGURED);

		hci_dev_set_flag(hdev, HCI_CONFIG);
		hci_dev_set_flag(hdev, HCI_AUTO_OFF);

		queue_work(hdev->req_workqueue, &hdev->power_on);
	}

unlock:
	hci_dev_unlock(hdev);
	return err;
}

static void read_local_oob_ext_data_complete(struct hci_dev *hdev, u8 status,
					     u16 opcode, struct sk_buff *skb)
{
	const struct mgmt_cp_read_local_oob_ext_data *mgmt_cp;
	struct mgmt_rp_read_local_oob_ext_data *mgmt_rp;
	u8 *h192, *r192, *h256, *r256;
	struct mgmt_pending_cmd *cmd;
	u16 eir_len;
	int err;

	BT_DBG("%s status %u", hdev->name, status);

	cmd = pending_find(MGMT_OP_READ_LOCAL_OOB_EXT_DATA, hdev);
	if (!cmd)
		return;

	mgmt_cp = cmd->param;

	if (status) {
		status = mgmt_status(status);
		eir_len = 0;

		h192 = NULL;
		r192 = NULL;
		h256 = NULL;
		r256 = NULL;
	} else if (opcode == HCI_OP_READ_LOCAL_OOB_DATA) {
		struct hci_rp_read_local_oob_data *rp;

		if (skb->len != sizeof(*rp)) {
			status = MGMT_STATUS_FAILED;
			eir_len = 0;
		} else {
			status = MGMT_STATUS_SUCCESS;
			rp = (void *)skb->data;

			eir_len = 5 + 18 + 18;
			h192 = rp->hash;
			r192 = rp->rand;
			h256 = NULL;
			r256 = NULL;
		}
	} else {
		struct hci_rp_read_local_oob_ext_data *rp;

		if (skb->len != sizeof(*rp)) {
			status = MGMT_STATUS_FAILED;
			eir_len = 0;
		} else {
			status = MGMT_STATUS_SUCCESS;
			rp = (void *)skb->data;

			if (hci_dev_test_flag(hdev, HCI_SC_ONLY)) {
				eir_len = 5 + 18 + 18;
				h192 = NULL;
				r192 = NULL;
			} else {
				eir_len = 5 + 18 + 18 + 18 + 18;
				h192 = rp->hash192;
				r192 = rp->rand192;
			}

			h256 = rp->hash256;
			r256 = rp->rand256;
		}
	}

	mgmt_rp = kmalloc(sizeof(*mgmt_rp) + eir_len, GFP_KERNEL);
	if (!mgmt_rp)
		goto done;

	if (status)
		goto send_rsp;

	eir_len = eir_append_data(mgmt_rp->eir, 0, EIR_CLASS_OF_DEV,
				  hdev->dev_class, 3);

	if (h192 && r192) {
		eir_len = eir_append_data(mgmt_rp->eir, eir_len,
					  EIR_SSP_HASH_C192, h192, 16);
		eir_len = eir_append_data(mgmt_rp->eir, eir_len,
					  EIR_SSP_RAND_R192, r192, 16);
	}

	if (h256 && r256) {
		eir_len = eir_append_data(mgmt_rp->eir, eir_len,
					  EIR_SSP_HASH_C256, h256, 16);
		eir_len = eir_append_data(mgmt_rp->eir, eir_len,
					  EIR_SSP_RAND_R256, r256, 16);
	}

send_rsp:
	mgmt_rp->type = mgmt_cp->type;
	mgmt_rp->eir_len = cpu_to_le16(eir_len);

	err = mgmt_cmd_complete(cmd->sk, hdev->id,
				MGMT_OP_READ_LOCAL_OOB_EXT_DATA, status,
				mgmt_rp, sizeof(*mgmt_rp) + eir_len);
	if (err < 0 || status)
		goto done;

	hci_sock_set_flag(cmd->sk, HCI_MGMT_OOB_DATA_EVENTS);

	err = mgmt_limited_event(MGMT_EV_LOCAL_OOB_DATA_UPDATED, hdev,
				 mgmt_rp, sizeof(*mgmt_rp) + eir_len,
				 HCI_MGMT_OOB_DATA_EVENTS, cmd->sk);
done:
	kfree(mgmt_rp);
	mgmt_pending_remove(cmd);
}

static int read_local_ssp_oob_req(struct hci_dev *hdev, struct sock *sk,
				  struct mgmt_cp_read_local_oob_ext_data *cp)
{
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;

	cmd = mgmt_pending_add(sk, MGMT_OP_READ_LOCAL_OOB_EXT_DATA, hdev,
			       cp, sizeof(*cp));
	if (!cmd)
		return -ENOMEM;

	hci_req_init(&req, hdev);

	if (bredr_sc_enabled(hdev))
		hci_req_add(&req, HCI_OP_READ_LOCAL_OOB_EXT_DATA, 0, NULL);
	else
		hci_req_add(&req, HCI_OP_READ_LOCAL_OOB_DATA, 0, NULL);

	err = hci_req_run_skb(&req, read_local_oob_ext_data_complete);
	if (err < 0) {
		mgmt_pending_remove(cmd);
		return err;
	}

	return 0;
}

static int read_local_oob_ext_data(struct sock *sk, struct hci_dev *hdev,
				   void *data, u16 data_len)
{
	struct mgmt_cp_read_local_oob_ext_data *cp = data;
	struct mgmt_rp_read_local_oob_ext_data *rp;
	size_t rp_len;
	u16 eir_len;
	u8 status, flags, role, addr[7], hash[16], rand[16];
	int err;

	BT_DBG("%s", hdev->name);

	if (hdev_is_powered(hdev)) {
		switch (cp->type) {
		case BIT(BDADDR_BREDR):
			status = mgmt_bredr_support(hdev);
			if (status)
				eir_len = 0;
			else
				eir_len = 5;
			break;
		case (BIT(BDADDR_LE_PUBLIC) | BIT(BDADDR_LE_RANDOM)):
			status = mgmt_le_support(hdev);
			if (status)
				eir_len = 0;
			else
				eir_len = 9 + 3 + 18 + 18 + 3;
			break;
		default:
			status = MGMT_STATUS_INVALID_PARAMS;
			eir_len = 0;
			break;
		}
	} else {
		status = MGMT_STATUS_NOT_POWERED;
		eir_len = 0;
	}

	rp_len = sizeof(*rp) + eir_len;
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp)
		return -ENOMEM;

	if (status)
		goto complete;

	hci_dev_lock(hdev);

	eir_len = 0;
	switch (cp->type) {
	case BIT(BDADDR_BREDR):
		if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED)) {
			err = read_local_ssp_oob_req(hdev, sk, cp);
			hci_dev_unlock(hdev);
			if (!err)
				goto done;

			status = MGMT_STATUS_FAILED;
			goto complete;
		} else {
			eir_len = eir_append_data(rp->eir, eir_len,
						  EIR_CLASS_OF_DEV,
						  hdev->dev_class, 3);
		}
		break;
	case (BIT(BDADDR_LE_PUBLIC) | BIT(BDADDR_LE_RANDOM)):
		if (hci_dev_test_flag(hdev, HCI_SC_ENABLED) &&
		    smp_generate_oob(hdev, hash, rand) < 0) {
			hci_dev_unlock(hdev);
			status = MGMT_STATUS_FAILED;
			goto complete;
		}

		/* This should return the active RPA, but since the RPA
		 * is only programmed on demand, it is really hard to fill
		 * this in at the moment. For now disallow retrieving
		 * local out-of-band data when privacy is in use.
		 *
		 * Returning the identity address will not help here since
		 * pairing happens before the identity resolving key is
		 * known and thus the connection establishment happens
		 * based on the RPA and not the identity address.
		 */
		if (hci_dev_test_flag(hdev, HCI_PRIVACY)) {
			hci_dev_unlock(hdev);
			status = MGMT_STATUS_REJECTED;
			goto complete;
		}

		if (hci_dev_test_flag(hdev, HCI_FORCE_STATIC_ADDR) ||
		   !bacmp(&hdev->bdaddr, BDADDR_ANY) ||
		   (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) &&
		    bacmp(&hdev->static_addr, BDADDR_ANY))) {
			memcpy(addr, &hdev->static_addr, 6);
			addr[6] = 0x01;
		} else {
			memcpy(addr, &hdev->bdaddr, 6);
			addr[6] = 0x00;
		}

		eir_len = eir_append_data(rp->eir, eir_len, EIR_LE_BDADDR,
					  addr, sizeof(addr));

		if (hci_dev_test_flag(hdev, HCI_ADVERTISING))
			role = 0x02;
		else
			role = 0x01;

		eir_len = eir_append_data(rp->eir, eir_len, EIR_LE_ROLE,
					  &role, sizeof(role));

		if (hci_dev_test_flag(hdev, HCI_SC_ENABLED)) {
			eir_len = eir_append_data(rp->eir, eir_len,
						  EIR_LE_SC_CONFIRM,
						  hash, sizeof(hash));

			eir_len = eir_append_data(rp->eir, eir_len,
						  EIR_LE_SC_RANDOM,
						  rand, sizeof(rand));
		}

		flags = mgmt_get_adv_discov_flags(hdev);

		if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
			flags |= LE_AD_NO_BREDR;

		eir_len = eir_append_data(rp->eir, eir_len, EIR_FLAGS,
					  &flags, sizeof(flags));
		break;
	}

	hci_dev_unlock(hdev);

	hci_sock_set_flag(sk, HCI_MGMT_OOB_DATA_EVENTS);

	status = MGMT_STATUS_SUCCESS;

complete:
	rp->type = cp->type;
	rp->eir_len = cpu_to_le16(eir_len);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_READ_LOCAL_OOB_EXT_DATA,
				status, rp, sizeof(*rp) + eir_len);
	if (err < 0 || status)
		goto done;

	err = mgmt_limited_event(MGMT_EV_LOCAL_OOB_DATA_UPDATED, hdev,
				 rp, sizeof(*rp) + eir_len,
				 HCI_MGMT_OOB_DATA_EVENTS, sk);

done:
	kfree(rp);

	return err;
}

static u32 get_supported_adv_flags(struct hci_dev *hdev)
{
	u32 flags = 0;

	flags |= MGMT_ADV_FLAG_CONNECTABLE;
	flags |= MGMT_ADV_FLAG_DISCOV;
	flags |= MGMT_ADV_FLAG_LIMITED_DISCOV;
	flags |= MGMT_ADV_FLAG_MANAGED_FLAGS;
	flags |= MGMT_ADV_FLAG_APPEARANCE;
	flags |= MGMT_ADV_FLAG_LOCAL_NAME;

	if (hdev->adv_tx_power != HCI_TX_POWER_INVALID)
		flags |= MGMT_ADV_FLAG_TX_POWER;

	return flags;
}

static int read_adv_features(struct sock *sk, struct hci_dev *hdev,
			     void *data, u16 data_len)
{
	struct mgmt_rp_read_adv_features *rp;
	size_t rp_len;
	int err;
	struct adv_info *adv_instance;
	u32 supported_flags;
	u8 *instance;

	BT_DBG("%s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_READ_ADV_FEATURES,
				       MGMT_STATUS_REJECTED);

	hci_dev_lock(hdev);

	rp_len = sizeof(*rp) + hdev->adv_instance_cnt;
	rp = kmalloc(rp_len, GFP_ATOMIC);
	if (!rp) {
		hci_dev_unlock(hdev);
		return -ENOMEM;
	}

	supported_flags = get_supported_adv_flags(hdev);

	rp->supported_flags = cpu_to_le32(supported_flags);
	rp->max_adv_data_len = HCI_MAX_AD_LENGTH;
	rp->max_scan_rsp_len = HCI_MAX_AD_LENGTH;
	rp->max_instances = HCI_MAX_ADV_INSTANCES;
	rp->num_instances = hdev->adv_instance_cnt;

	instance = rp->instance;
	list_for_each_entry(adv_instance, &hdev->adv_instances, list) {
		*instance = adv_instance->instance;
		instance++;
	}

	hci_dev_unlock(hdev);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_READ_ADV_FEATURES,
				MGMT_STATUS_SUCCESS, rp, rp_len);

	kfree(rp);

	return err;
}

static u8 calculate_name_len(struct hci_dev *hdev)
{
	u8 buf[HCI_MAX_SHORT_NAME_LENGTH + 3];

	return append_local_name(hdev, buf, 0);
}

static u8 tlv_data_max_len(struct hci_dev *hdev, u32 adv_flags,
			   bool is_adv_data)
{
	u8 max_len = HCI_MAX_AD_LENGTH;

	if (is_adv_data) {
		if (adv_flags & (MGMT_ADV_FLAG_DISCOV |
				 MGMT_ADV_FLAG_LIMITED_DISCOV |
				 MGMT_ADV_FLAG_MANAGED_FLAGS))
			max_len -= 3;

		if (adv_flags & MGMT_ADV_FLAG_TX_POWER)
			max_len -= 3;
	} else {
		if (adv_flags & MGMT_ADV_FLAG_LOCAL_NAME)
			max_len -= calculate_name_len(hdev);

		if (adv_flags & (MGMT_ADV_FLAG_APPEARANCE))
			max_len -= 4;
	}

	return max_len;
}

static bool flags_managed(u32 adv_flags)
{
	return adv_flags & (MGMT_ADV_FLAG_DISCOV |
			    MGMT_ADV_FLAG_LIMITED_DISCOV |
			    MGMT_ADV_FLAG_MANAGED_FLAGS);
}

static bool tx_power_managed(u32 adv_flags)
{
	return adv_flags & MGMT_ADV_FLAG_TX_POWER;
}

static bool name_managed(u32 adv_flags)
{
	return adv_flags & MGMT_ADV_FLAG_LOCAL_NAME;
}

static bool appearance_managed(u32 adv_flags)
{
	return adv_flags & MGMT_ADV_FLAG_APPEARANCE;
}

static bool tlv_data_is_valid(struct hci_dev *hdev, u32 adv_flags, u8 *data,
			      u8 len, bool is_adv_data)
{
	int i, cur_len;
	u8 max_len;

	max_len = tlv_data_max_len(hdev, adv_flags, is_adv_data);

	if (len > max_len)
		return false;

	/* Make sure that the data is correctly formatted. */
	for (i = 0, cur_len = 0; i < len; i += (cur_len + 1)) {
		cur_len = data[i];

		if (data[i + 1] == EIR_FLAGS &&
		    (!is_adv_data || flags_managed(adv_flags)))
			return false;

		if (data[i + 1] == EIR_TX_POWER && tx_power_managed(adv_flags))
			return false;

		if (data[i + 1] == EIR_NAME_COMPLETE && name_managed(adv_flags))
			return false;

		if (data[i + 1] == EIR_NAME_SHORT && name_managed(adv_flags))
			return false;

		if (data[i + 1] == EIR_APPEARANCE &&
		    appearance_managed(adv_flags))
			return false;

		/* If the current field length would exceed the total data
		 * length, then it's invalid.
		 */
		if (i + cur_len >= len)
			return false;
	}

	return true;
}

static void add_advertising_complete(struct hci_dev *hdev, u8 status,
				     u16 opcode)
{
	struct mgmt_pending_cmd *cmd;
	struct mgmt_cp_add_advertising *cp;
	struct mgmt_rp_add_advertising rp;
	struct adv_info *adv_instance, *n;
	u8 instance;

	BT_DBG("status %d", status);

	hci_dev_lock(hdev);

	cmd = pending_find(MGMT_OP_ADD_ADVERTISING, hdev);

	list_for_each_entry_safe(adv_instance, n, &hdev->adv_instances, list) {
		if (!adv_instance->pending)
			continue;

		if (!status) {
			adv_instance->pending = false;
			continue;
		}

		instance = adv_instance->instance;

		if (hdev->cur_adv_instance == instance)
			cancel_adv_timeout(hdev);

		hci_remove_adv_instance(hdev, instance);
		mgmt_advertising_removed(cmd ? cmd->sk : NULL, hdev, instance);
	}

	if (!cmd)
		goto unlock;

	cp = cmd->param;
	rp.instance = cp->instance;

	if (status)
		mgmt_cmd_status(cmd->sk, cmd->index, cmd->opcode,
				mgmt_status(status));
	else
		mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode,
				  mgmt_status(status), &rp, sizeof(rp));

	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int add_advertising(struct sock *sk, struct hci_dev *hdev,
			   void *data, u16 data_len)
{
	struct mgmt_cp_add_advertising *cp = data;
	struct mgmt_rp_add_advertising rp;
	u32 flags;
	u32 supported_flags;
	u8 status;
	u16 timeout, duration;
	unsigned int prev_instance_cnt = hdev->adv_instance_cnt;
	u8 schedule_instance = 0;
	struct adv_info *next_instance;
	int err;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;

	BT_DBG("%s", hdev->name);

	status = mgmt_le_support(hdev);
	if (status)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				       status);

	if (cp->instance < 1 || cp->instance > HCI_MAX_ADV_INSTANCES)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				       MGMT_STATUS_INVALID_PARAMS);

	if (data_len != sizeof(*cp) + cp->adv_data_len + cp->scan_rsp_len)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				       MGMT_STATUS_INVALID_PARAMS);

	flags = __le32_to_cpu(cp->flags);
	timeout = __le16_to_cpu(cp->timeout);
	duration = __le16_to_cpu(cp->duration);

	/* The current implementation only supports a subset of the specified
	 * flags.
	 */
	supported_flags = get_supported_adv_flags(hdev);
	if (flags & ~supported_flags)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				       MGMT_STATUS_INVALID_PARAMS);

	hci_dev_lock(hdev);

	if (timeout && !hdev_is_powered(hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				      MGMT_STATUS_REJECTED);
		goto unlock;
	}

	if (pending_find(MGMT_OP_ADD_ADVERTISING, hdev) ||
	    pending_find(MGMT_OP_REMOVE_ADVERTISING, hdev) ||
	    pending_find(MGMT_OP_SET_LE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if (!tlv_data_is_valid(hdev, flags, cp->data, cp->adv_data_len, true) ||
	    !tlv_data_is_valid(hdev, flags, cp->data + cp->adv_data_len,
			       cp->scan_rsp_len, false)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}

	err = hci_add_adv_instance(hdev, cp->instance, flags,
				   cp->adv_data_len, cp->data,
				   cp->scan_rsp_len,
				   cp->data + cp->adv_data_len,
				   timeout, duration);
	if (err < 0) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
				      MGMT_STATUS_FAILED);
		goto unlock;
	}

	/* Only trigger an advertising added event if a new instance was
	 * actually added.
	 */
	if (hdev->adv_instance_cnt > prev_instance_cnt)
		mgmt_advertising_added(sk, hdev, cp->instance);

	if (hdev->cur_adv_instance == cp->instance) {
		/* If the currently advertised instance is being changed then
		 * cancel the current advertising and schedule the next
		 * instance. If there is only one instance then the overridden
		 * advertising data will be visible right away.
		 */
		cancel_adv_timeout(hdev);

		next_instance = hci_get_next_instance(hdev, cp->instance);
		if (next_instance)
			schedule_instance = next_instance->instance;
	} else if (!hdev->adv_instance_timeout) {
		/* Immediately advertise the new instance if no other
		 * instance is currently being advertised.
		 */
		schedule_instance = cp->instance;
	}

	/* If the HCI_ADVERTISING flag is set or the device isn't powered or
	 * there is no instance to be advertised then we have no HCI
	 * communication to make. Simply return.
	 */
	if (!hdev_is_powered(hdev) ||
	    hci_dev_test_flag(hdev, HCI_ADVERTISING) ||
	    !schedule_instance) {
		rp.instance = cp->instance;
		err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_ADD_ADVERTISING,
					MGMT_STATUS_SUCCESS, &rp, sizeof(rp));
		goto unlock;
	}

	/* We're good to go, update advertising data, parameters, and start
	 * advertising.
	 */
	cmd = mgmt_pending_add(sk, MGMT_OP_ADD_ADVERTISING, hdev, data,
			       data_len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	hci_req_init(&req, hdev);

	err = __hci_req_schedule_adv_instance(&req, schedule_instance, true);

	if (!err)
		err = hci_req_run(&req, add_advertising_complete);

	if (err < 0)
		mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);

	return err;
}

static void remove_advertising_complete(struct hci_dev *hdev, u8 status,
					u16 opcode)
{
	struct mgmt_pending_cmd *cmd;
	struct mgmt_cp_remove_advertising *cp;
	struct mgmt_rp_remove_advertising rp;

	BT_DBG("status %d", status);

	hci_dev_lock(hdev);

	/* A failure status here only means that we failed to disable
	 * advertising. Otherwise, the advertising instance has been removed,
	 * so report success.
	 */
	cmd = pending_find(MGMT_OP_REMOVE_ADVERTISING, hdev);
	if (!cmd)
		goto unlock;

	cp = cmd->param;
	rp.instance = cp->instance;

	mgmt_cmd_complete(cmd->sk, cmd->index, cmd->opcode, MGMT_STATUS_SUCCESS,
			  &rp, sizeof(rp));
	mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);
}

static int remove_advertising(struct sock *sk, struct hci_dev *hdev,
			      void *data, u16 data_len)
{
	struct mgmt_cp_remove_advertising *cp = data;
	struct mgmt_rp_remove_advertising rp;
	struct mgmt_pending_cmd *cmd;
	struct hci_request req;
	int err;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (cp->instance && !hci_find_adv_instance(hdev, cp->instance)) {
		err = mgmt_cmd_status(sk, hdev->id,
				      MGMT_OP_REMOVE_ADVERTISING,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}

	if (pending_find(MGMT_OP_ADD_ADVERTISING, hdev) ||
	    pending_find(MGMT_OP_REMOVE_ADVERTISING, hdev) ||
	    pending_find(MGMT_OP_SET_LE, hdev)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_REMOVE_ADVERTISING,
				      MGMT_STATUS_BUSY);
		goto unlock;
	}

	if (list_empty(&hdev->adv_instances)) {
		err = mgmt_cmd_status(sk, hdev->id, MGMT_OP_REMOVE_ADVERTISING,
				      MGMT_STATUS_INVALID_PARAMS);
		goto unlock;
	}

	hci_req_init(&req, hdev);

	hci_req_clear_adv_instance(hdev, sk, &req, cp->instance, true);

	if (list_empty(&hdev->adv_instances))
		__hci_req_disable_advertising(&req);

	/* If no HCI commands have been collected so far or the HCI_ADVERTISING
	 * flag is set or the device isn't powered then we have no HCI
	 * communication to make. Simply return.
	 */
	if (skb_queue_empty(&req.cmd_q) ||
	    !hdev_is_powered(hdev) ||
	    hci_dev_test_flag(hdev, HCI_ADVERTISING)) {
		rp.instance = cp->instance;
		err = mgmt_cmd_complete(sk, hdev->id,
					MGMT_OP_REMOVE_ADVERTISING,
					MGMT_STATUS_SUCCESS, &rp, sizeof(rp));
		goto unlock;
	}

	cmd = mgmt_pending_add(sk, MGMT_OP_REMOVE_ADVERTISING, hdev, data,
			       data_len);
	if (!cmd) {
		err = -ENOMEM;
		goto unlock;
	}

	err = hci_req_run(&req, remove_advertising_complete);
	if (err < 0)
		mgmt_pending_remove(cmd);

unlock:
	hci_dev_unlock(hdev);

	return err;
}

static int get_adv_size_info(struct sock *sk, struct hci_dev *hdev,
			     void *data, u16 data_len)
{
	struct mgmt_cp_get_adv_size_info *cp = data;
	struct mgmt_rp_get_adv_size_info rp;
	u32 flags, supported_flags;
	int err;

	BT_DBG("%s", hdev->name);

	if (!lmp_le_capable(hdev))
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_GET_ADV_SIZE_INFO,
				       MGMT_STATUS_REJECTED);

	if (cp->instance < 1 || cp->instance > HCI_MAX_ADV_INSTANCES)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_GET_ADV_SIZE_INFO,
				       MGMT_STATUS_INVALID_PARAMS);

	flags = __le32_to_cpu(cp->flags);

	/* The current implementation only supports a subset of the specified
	 * flags.
	 */
	supported_flags = get_supported_adv_flags(hdev);
	if (flags & ~supported_flags)
		return mgmt_cmd_status(sk, hdev->id, MGMT_OP_GET_ADV_SIZE_INFO,
				       MGMT_STATUS_INVALID_PARAMS);

	rp.instance = cp->instance;
	rp.flags = cp->flags;
	rp.max_adv_data_len = tlv_data_max_len(hdev, flags, true);
	rp.max_scan_rsp_len = tlv_data_max_len(hdev, flags, false);

	err = mgmt_cmd_complete(sk, hdev->id, MGMT_OP_GET_ADV_SIZE_INFO,
				MGMT_STATUS_SUCCESS, &rp, sizeof(rp));

	return err;
}

static const struct hci_mgmt_handler mgmt_handlers[] = {
	{ NULL }, /* 0x0000 (no command) */
	{ read_version,            MGMT_READ_VERSION_SIZE,
						HCI_MGMT_NO_HDEV |
						HCI_MGMT_UNTRUSTED },
	{ read_commands,           MGMT_READ_COMMANDS_SIZE,
						HCI_MGMT_NO_HDEV |
						HCI_MGMT_UNTRUSTED },
	{ read_index_list,         MGMT_READ_INDEX_LIST_SIZE,
						HCI_MGMT_NO_HDEV |
						HCI_MGMT_UNTRUSTED },
	{ read_controller_info,    MGMT_READ_INFO_SIZE,
						HCI_MGMT_UNTRUSTED },
	{ set_powered,             MGMT_SETTING_SIZE },
	{ set_discoverable,        MGMT_SET_DISCOVERABLE_SIZE },
	{ set_connectable,         MGMT_SETTING_SIZE },
	{ set_fast_connectable,    MGMT_SETTING_SIZE },
	{ set_bondable,            MGMT_SETTING_SIZE },
	{ set_link_security,       MGMT_SETTING_SIZE },
	{ set_ssp,                 MGMT_SETTING_SIZE },
	{ set_hs,                  MGMT_SETTING_SIZE },
	{ set_le,                  MGMT_SETTING_SIZE },
	{ set_dev_class,           MGMT_SET_DEV_CLASS_SIZE },
	{ set_local_name,          MGMT_SET_LOCAL_NAME_SIZE },
	{ add_uuid,                MGMT_ADD_UUID_SIZE },
	{ remove_uuid,             MGMT_REMOVE_UUID_SIZE },
	{ load_link_keys,          MGMT_LOAD_LINK_KEYS_SIZE,
						HCI_MGMT_VAR_LEN },
	{ load_long_term_keys,     MGMT_LOAD_LONG_TERM_KEYS_SIZE,
						HCI_MGMT_VAR_LEN },
	{ disconnect,              MGMT_DISCONNECT_SIZE },
	{ get_connections,         MGMT_GET_CONNECTIONS_SIZE },
	{ pin_code_reply,          MGMT_PIN_CODE_REPLY_SIZE },
	{ pin_code_neg_reply,      MGMT_PIN_CODE_NEG_REPLY_SIZE },
	{ set_io_capability,       MGMT_SET_IO_CAPABILITY_SIZE },
	{ pair_device,             MGMT_PAIR_DEVICE_SIZE },
	{ cancel_pair_device,      MGMT_CANCEL_PAIR_DEVICE_SIZE },
	{ unpair_device,           MGMT_UNPAIR_DEVICE_SIZE },
	{ user_confirm_reply,      MGMT_USER_CONFIRM_REPLY_SIZE },
	{ user_confirm_neg_reply,  MGMT_USER_CONFIRM_NEG_REPLY_SIZE },
	{ user_passkey_reply,      MGMT_USER_PASSKEY_REPLY_SIZE },
	{ user_passkey_neg_reply,  MGMT_USER_PASSKEY_NEG_REPLY_SIZE },
	{ read_local_oob_data,     MGMT_READ_LOCAL_OOB_DATA_SIZE },
	{ add_remote_oob_data,     MGMT_ADD_REMOTE_OOB_DATA_SIZE,
						HCI_MGMT_VAR_LEN },
	{ remove_remote_oob_data,  MGMT_REMOVE_REMOTE_OOB_DATA_SIZE },
	{ start_discovery,         MGMT_START_DISCOVERY_SIZE },
	{ stop_discovery,          MGMT_STOP_DISCOVERY_SIZE },
	{ confirm_name,            MGMT_CONFIRM_NAME_SIZE },
	{ block_device,            MGMT_BLOCK_DEVICE_SIZE },
	{ unblock_device,          MGMT_UNBLOCK_DEVICE_SIZE },
	{ set_device_id,           MGMT_SET_DEVICE_ID_SIZE },
	{ set_advertising,         MGMT_SETTING_SIZE },
	{ set_bredr,               MGMT_SETTING_SIZE },
	{ set_static_address,      MGMT_SET_STATIC_ADDRESS_SIZE },
	{ set_scan_params,         MGMT_SET_SCAN_PARAMS_SIZE },
	{ set_secure_conn,         MGMT_SETTING_SIZE },
	{ set_debug_keys,          MGMT_SETTING_SIZE },
	{ set_privacy,             MGMT_SET_PRIVACY_SIZE },
	{ load_irks,               MGMT_LOAD_IRKS_SIZE,
						HCI_MGMT_VAR_LEN },
	{ get_conn_info,           MGMT_GET_CONN_INFO_SIZE },
	{ get_clock_info,          MGMT_GET_CLOCK_INFO_SIZE },
	{ add_device,              MGMT_ADD_DEVICE_SIZE },
	{ remove_device,           MGMT_REMOVE_DEVICE_SIZE },
	{ load_conn_param,         MGMT_LOAD_CONN_PARAM_SIZE,
						HCI_MGMT_VAR_LEN },
	{ read_unconf_index_list,  MGMT_READ_UNCONF_INDEX_LIST_SIZE,
						HCI_MGMT_NO_HDEV |
						HCI_MGMT_UNTRUSTED },
	{ read_config_info,        MGMT_READ_CONFIG_INFO_SIZE,
						HCI_MGMT_UNCONFIGURED |
						HCI_MGMT_UNTRUSTED },
	{ set_external_config,     MGMT_SET_EXTERNAL_CONFIG_SIZE,
						HCI_MGMT_UNCONFIGURED },
	{ set_public_address,      MGMT_SET_PUBLIC_ADDRESS_SIZE,
						HCI_MGMT_UNCONFIGURED },
	{ start_service_discovery, MGMT_START_SERVICE_DISCOVERY_SIZE,
						HCI_MGMT_VAR_LEN },
	{ read_local_oob_ext_data, MGMT_READ_LOCAL_OOB_EXT_DATA_SIZE },
	{ read_ext_index_list,     MGMT_READ_EXT_INDEX_LIST_SIZE,
						HCI_MGMT_NO_HDEV |
						HCI_MGMT_UNTRUSTED },
	{ read_adv_features,       MGMT_READ_ADV_FEATURES_SIZE },
	{ add_advertising,	   MGMT_ADD_ADVERTISING_SIZE,
						HCI_MGMT_VAR_LEN },
	{ remove_advertising,	   MGMT_REMOVE_ADVERTISING_SIZE },
	{ get_adv_size_info,       MGMT_GET_ADV_SIZE_INFO_SIZE },
	{ start_limited_discovery, MGMT_START_DISCOVERY_SIZE },
	{ read_ext_controller_info,MGMT_READ_EXT_INFO_SIZE,
						HCI_MGMT_UNTRUSTED },
	{ set_appearance,	   MGMT_SET_APPEARANCE_SIZE },
};

void mgmt_index_added(struct hci_dev *hdev)
{
	struct mgmt_ev_ext_index ev;

	if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
		return;

	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
			mgmt_index_event(MGMT_EV_UNCONF_INDEX_ADDED, hdev,
					 NULL, 0, HCI_MGMT_UNCONF_INDEX_EVENTS);
			ev.type = 0x01;
		} else {
			mgmt_index_event(MGMT_EV_INDEX_ADDED, hdev, NULL, 0,
					 HCI_MGMT_INDEX_EVENTS);
			ev.type = 0x00;
		}
		break;
	case HCI_AMP:
		ev.type = 0x02;
		break;
	default:
		return;
	}

	ev.bus = hdev->bus;

	mgmt_index_event(MGMT_EV_EXT_INDEX_ADDED, hdev, &ev, sizeof(ev),
			 HCI_MGMT_EXT_INDEX_EVENTS);
}

void mgmt_index_removed(struct hci_dev *hdev)
{
	struct mgmt_ev_ext_index ev;
	u8 status = MGMT_STATUS_INVALID_INDEX;

	if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
		return;

	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		mgmt_pending_foreach(0, hdev, cmd_complete_rsp, &status);

		if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
			mgmt_index_event(MGMT_EV_UNCONF_INDEX_REMOVED, hdev,
					 NULL, 0, HCI_MGMT_UNCONF_INDEX_EVENTS);
			ev.type = 0x01;
		} else {
			mgmt_index_event(MGMT_EV_INDEX_REMOVED, hdev, NULL, 0,
					 HCI_MGMT_INDEX_EVENTS);
			ev.type = 0x00;
		}
		break;
	case HCI_AMP:
		ev.type = 0x02;
		break;
	default:
		return;
	}

	ev.bus = hdev->bus;

	mgmt_index_event(MGMT_EV_EXT_INDEX_REMOVED, hdev, &ev, sizeof(ev),
			 HCI_MGMT_EXT_INDEX_EVENTS);
}

/* This function requires the caller holds hdev->lock */
static void restart_le_actions(struct hci_dev *hdev)
{
	struct hci_conn_params *p;

	list_for_each_entry(p, &hdev->le_conn_params, list) {
		/* Needed for AUTO_OFF case where might not "really"
		 * have been powered off.
		 */
		list_del_init(&p->action);

		switch (p->auto_connect) {
		case HCI_AUTO_CONN_DIRECT:
		case HCI_AUTO_CONN_ALWAYS:
			list_add(&p->action, &hdev->pend_le_conns);
			break;
		case HCI_AUTO_CONN_REPORT:
			list_add(&p->action, &hdev->pend_le_reports);
			break;
		default:
			break;
		}
	}
}

void mgmt_power_on(struct hci_dev *hdev, int err)
{
	struct cmd_lookup match = { NULL, hdev };

	BT_DBG("err %d", err);

	hci_dev_lock(hdev);

	if (!err) {
		restart_le_actions(hdev);
		hci_update_background_scan(hdev);
	}

	mgmt_pending_foreach(MGMT_OP_SET_POWERED, hdev, settings_rsp, &match);

	new_settings(hdev, match.sk);
>>>>>>> android-4.9

	if (match.sk)
		sock_put(match.sk);

<<<<<<< HEAD
	return ret;
}

int mgmt_new_key(u16 index, struct link_key *key, u8 bonded)
{
	struct mgmt_ev_new_key *ev;
	int err, total;

	total = sizeof(struct mgmt_ev_new_key) + key->dlen;
	ev = kzalloc(total, GFP_ATOMIC);
	if (!ev)
		return -ENOMEM;

	bacpy(&ev->key.bdaddr, &key->bdaddr);
	ev->key.addr_type = key->addr_type;
	ev->key.key_type = key->key_type;
	memcpy(ev->key.val, key->val, 16);
	ev->key.pin_len = key->pin_len;
	ev->key.auth = key->auth;
	ev->store_hint = bonded;
	ev->key.dlen = key->dlen;

	memcpy(ev->key.data, key->data, key->dlen);
=======
	hci_dev_unlock(hdev);
}

void __mgmt_power_off(struct hci_dev *hdev)
{
	struct cmd_lookup match = { NULL, hdev };
	u8 status, zero_cod[] = { 0, 0, 0 };

	mgmt_pending_foreach(MGMT_OP_SET_POWERED, hdev, settings_rsp, &match);

	/* If the power off is because of hdev unregistration let
	 * use the appropriate INVALID_INDEX status. Otherwise use
	 * NOT_POWERED. We cover both scenarios here since later in
	 * mgmt_index_removed() any hci_conn callbacks will have already
	 * been triggered, potentially causing misleading DISCONNECTED
	 * status responses.
	 */
	if (hci_dev_test_flag(hdev, HCI_UNREGISTER))
		status = MGMT_STATUS_INVALID_INDEX;
	else
		status = MGMT_STATUS_NOT_POWERED;

	mgmt_pending_foreach(0, hdev, cmd_complete_rsp, &status);

	if (memcmp(hdev->dev_class, zero_cod, sizeof(zero_cod)) != 0) {
		mgmt_limited_event(MGMT_EV_CLASS_OF_DEV_CHANGED, hdev,
				   zero_cod, sizeof(zero_cod),
				   HCI_MGMT_DEV_CLASS_EVENTS, NULL);
		ext_info_changed(hdev, NULL);
	}

	new_settings(hdev, match.sk);

	if (match.sk)
		sock_put(match.sk);
}

void mgmt_set_powered_failed(struct hci_dev *hdev, int err)
{
	struct mgmt_pending_cmd *cmd;
	u8 status;

	cmd = pending_find(MGMT_OP_SET_POWERED, hdev);
	if (!cmd)
		return;

	if (err == -ERFKILL)
		status = MGMT_STATUS_RFKILLED;
	else
		status = MGMT_STATUS_FAILED;

	mgmt_cmd_status(cmd->sk, hdev->id, MGMT_OP_SET_POWERED, status);

	mgmt_pending_remove(cmd);
}

void mgmt_new_link_key(struct hci_dev *hdev, struct link_key *key,
		       bool persistent)
{
	struct mgmt_ev_new_link_key ev;
>>>>>>> android-4.9

	err = mgmt_event(MGMT_EV_NEW_KEY, index, ev, total, NULL);

<<<<<<< HEAD
	kfree(ev);

	return err;
}

int mgmt_connected(u16 index, bdaddr_t *bdaddr, u8 le)
=======
	ev.store_hint = persistent;
	bacpy(&ev.key.addr.bdaddr, &key->bdaddr);
	ev.key.addr.type = BDADDR_BREDR;
	ev.key.type = key->type;
	memcpy(ev.key.val, key->val, HCI_LINK_KEY_SIZE);
	ev.key.pin_len = key->pin_len;

	mgmt_event(MGMT_EV_NEW_LINK_KEY, hdev, &ev, sizeof(ev), NULL);
}

static u8 mgmt_ltk_type(struct smp_ltk *ltk)
{
	switch (ltk->type) {
	case SMP_LTK:
	case SMP_LTK_SLAVE:
		if (ltk->authenticated)
			return MGMT_LTK_AUTHENTICATED;
		return MGMT_LTK_UNAUTHENTICATED;
	case SMP_LTK_P256:
		if (ltk->authenticated)
			return MGMT_LTK_P256_AUTH;
		return MGMT_LTK_P256_UNAUTH;
	case SMP_LTK_P256_DEBUG:
		return MGMT_LTK_P256_DEBUG;
	}

	return MGMT_LTK_UNAUTHENTICATED;
}

void mgmt_new_ltk(struct hci_dev *hdev, struct smp_ltk *key, bool persistent)
>>>>>>> android-4.9
{
	struct mgmt_ev_connected ev;
	struct pending_cmd *cmd;
	struct hci_dev *hdev;

	BT_DBG("hci%u", index);

<<<<<<< HEAD
	hdev = hci_dev_get(index);

	if (!hdev)
		return -ENODEV;

	bacpy(&ev.bdaddr, bdaddr);
	ev.le = le;

	cmd = mgmt_pending_find(MGMT_OP_LE_CREATE_CONN_WHITE_LIST, index);
	if (cmd) {
		BT_ERR("mgmt_connected remove mgmt pending white_list");
		mgmt_pending_remove(cmd);
	}

	return mgmt_event(MGMT_EV_CONNECTED, index, &ev, sizeof(ev), NULL);
}

int mgmt_le_conn_params(u16 index, bdaddr_t *bdaddr, u16 interval,
						u16 latency, u16 timeout)
{
	struct mgmt_ev_le_conn_params ev;

	bacpy(&ev.bdaddr, bdaddr);
	ev.interval = interval;
	ev.latency = latency;
	ev.timeout = timeout;

	return mgmt_event(MGMT_EV_LE_CONN_PARAMS, index, &ev, sizeof(ev),
									NULL);
=======
	/* Devices using resolvable or non-resolvable random addresses
	 * without providing an identity resolving key don't require
	 * to store long term keys. Their addresses will change the
	 * next time around.
	 *
	 * Only when a remote device provides an identity address
	 * make sure the long term key is stored. If the remote
	 * identity is known, the long term keys are internally
	 * mapped to the identity address. So allow static random
	 * and public addresses here.
	 */
	if (key->bdaddr_type == ADDR_LE_DEV_RANDOM &&
	    (key->bdaddr.b[5] & 0xc0) != 0xc0)
		ev.store_hint = 0x00;
	else
		ev.store_hint = persistent;

	bacpy(&ev.key.addr.bdaddr, &key->bdaddr);
	ev.key.addr.type = link_to_bdaddr(LE_LINK, key->bdaddr_type);
	ev.key.type = mgmt_ltk_type(key);
	ev.key.enc_size = key->enc_size;
	ev.key.ediv = key->ediv;
	ev.key.rand = key->rand;

	if (key->type == SMP_LTK)
		ev.key.master = 1;

	/* Make sure we copy only the significant bytes based on the
	 * encryption key size, and set the rest of the value to zeroes.
	 */
	memcpy(ev.key.val, key->val, key->enc_size);
	memset(ev.key.val + key->enc_size, 0,
	       sizeof(ev.key.val) - key->enc_size);

	mgmt_event(MGMT_EV_NEW_LONG_TERM_KEY, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_new_irk(struct hci_dev *hdev, struct smp_irk *irk, bool persistent)
{
	struct mgmt_ev_new_irk ev;

	memset(&ev, 0, sizeof(ev));

	ev.store_hint = persistent;

	bacpy(&ev.rpa, &irk->rpa);
	bacpy(&ev.irk.addr.bdaddr, &irk->bdaddr);
	ev.irk.addr.type = link_to_bdaddr(LE_LINK, irk->addr_type);
	memcpy(ev.irk.val, irk->val, sizeof(irk->val));

	mgmt_event(MGMT_EV_NEW_IRK, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_new_csrk(struct hci_dev *hdev, struct smp_csrk *csrk,
		   bool persistent)
{
	struct mgmt_ev_new_csrk ev;

	memset(&ev, 0, sizeof(ev));

	/* Devices using resolvable or non-resolvable random addresses
	 * without providing an identity resolving key don't require
	 * to store signature resolving keys. Their addresses will change
	 * the next time around.
	 *
	 * Only when a remote device provides an identity address
	 * make sure the signature resolving key is stored. So allow
	 * static random and public addresses here.
	 */
	if (csrk->bdaddr_type == ADDR_LE_DEV_RANDOM &&
	    (csrk->bdaddr.b[5] & 0xc0) != 0xc0)
		ev.store_hint = 0x00;
	else
		ev.store_hint = persistent;

	bacpy(&ev.key.addr.bdaddr, &csrk->bdaddr);
	ev.key.addr.type = link_to_bdaddr(LE_LINK, csrk->bdaddr_type);
	ev.key.type = csrk->type;
	memcpy(ev.key.val, csrk->val, sizeof(csrk->val));

	mgmt_event(MGMT_EV_NEW_CSRK, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_new_conn_param(struct hci_dev *hdev, bdaddr_t *bdaddr,
			 u8 bdaddr_type, u8 store_hint, u16 min_interval,
			 u16 max_interval, u16 latency, u16 timeout)
{
	struct mgmt_ev_new_conn_param ev;

	if (!hci_is_identity_address(bdaddr, bdaddr_type))
		return;

	memset(&ev, 0, sizeof(ev));
	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(LE_LINK, bdaddr_type);
	ev.store_hint = store_hint;
	ev.min_interval = cpu_to_le16(min_interval);
	ev.max_interval = cpu_to_le16(max_interval);
	ev.latency = cpu_to_le16(latency);
	ev.timeout = cpu_to_le16(timeout);

	mgmt_event(MGMT_EV_NEW_CONN_PARAM, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_device_connected(struct hci_dev *hdev, struct hci_conn *conn,
			   u32 flags, u8 *name, u8 name_len)
{
	char buf[512];
	struct mgmt_ev_device_connected *ev = (void *) buf;
	u16 eir_len = 0;

	bacpy(&ev->addr.bdaddr, &conn->dst);
	ev->addr.type = link_to_bdaddr(conn->type, conn->dst_type);

	ev->flags = __cpu_to_le32(flags);

	/* We must ensure that the EIR Data fields are ordered and
	 * unique. Keep it simple for now and avoid the problem by not
	 * adding any BR/EDR data to the LE adv.
	 */
	if (conn->le_adv_data_len > 0) {
		memcpy(&ev->eir[eir_len],
		       conn->le_adv_data, conn->le_adv_data_len);
		eir_len = conn->le_adv_data_len;
	} else {
		if (name_len > 0)
			eir_len = eir_append_data(ev->eir, 0, EIR_NAME_COMPLETE,
						  name, name_len);

		if (memcmp(conn->dev_class, "\0\0\0", 3) != 0)
			eir_len = eir_append_data(ev->eir, eir_len,
						  EIR_CLASS_OF_DEV,
						  conn->dev_class, 3);
	}

	ev->eir_len = cpu_to_le16(eir_len);

	mgmt_event(MGMT_EV_DEVICE_CONNECTED, hdev, buf,
		    sizeof(*ev) + eir_len, NULL);
>>>>>>> android-4.9
}

static void disconnect_rsp(struct mgmt_pending_cmd *cmd, void *data)
{
	struct sock **sk = data;
<<<<<<< HEAD
	struct mgmt_rp_disconnect rp;

	bacpy(&rp.bdaddr, &cp->bdaddr);

	cmd_complete(cmd->sk, cmd->index, MGMT_OP_DISCONNECT, &rp, sizeof(rp));
=======

	cmd->cmd_complete(cmd, 0);
>>>>>>> android-4.9

	*sk = cmd->sk;
	sock_hold(*sk);

	mgmt_pending_remove(cmd);
}

<<<<<<< HEAD
int mgmt_disconnected(u16 index, bdaddr_t *bdaddr, u8 reason)
{
	struct mgmt_ev_disconnected ev;
=======
static void unpair_device_rsp(struct mgmt_pending_cmd *cmd, void *data)
{
	struct hci_dev *hdev = data;
	struct mgmt_cp_unpair_device *cp = cmd->param;

	device_unpaired(hdev, &cp->addr.bdaddr, cp->addr.type, cmd->sk);

	cmd->cmd_complete(cmd, 0);
	mgmt_pending_remove(cmd);
}

bool mgmt_powering_down(struct hci_dev *hdev)
{
	struct mgmt_pending_cmd *cmd;
	struct mgmt_mode *cp;

	cmd = pending_find(MGMT_OP_SET_POWERED, hdev);
	if (!cmd)
		return false;

	cp = cmd->param;
	if (!cp->val)
		return true;

	return false;
}

void mgmt_device_disconnected(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type, u8 reason,
			      bool mgmt_connected)
{
	struct mgmt_ev_device_disconnected ev;
>>>>>>> android-4.9
	struct sock *sk = NULL;

	/* The connection is still in hci_conn_hash so test for 1
	 * instead of 0 to know if this is the last one.
	 */
	if (mgmt_powering_down(hdev) && hci_conn_count(hdev) == 1) {
		cancel_delayed_work(&hdev->power_off);
		queue_work(hdev->req_workqueue, &hdev->power_off.work);
	}

	if (!mgmt_connected)
		return;

	if (link_type != ACL_LINK && link_type != LE_LINK)
		return;

<<<<<<< HEAD
	bacpy(&ev.bdaddr, bdaddr);
	ev.reason = reason;

	err = mgmt_event(MGMT_EV_DISCONNECTED, index, &ev, sizeof(ev), sk);
=======
	mgmt_pending_foreach(MGMT_OP_DISCONNECT, hdev, disconnect_rsp, &sk);

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(link_type, addr_type);
	ev.reason = reason;

	mgmt_event(MGMT_EV_DEVICE_DISCONNECTED, hdev, &ev, sizeof(ev), sk);
>>>>>>> android-4.9

	if (sk)
		sock_put(sk);

<<<<<<< HEAD
	mgmt_pending_foreach(MGMT_OP_DISCONNECT, index, disconnect_rsp, &sk);

	return err;
}

int mgmt_disconnect_failed(u16 index)
{
	struct pending_cmd *cmd;
	int err;

	cmd = mgmt_pending_find(MGMT_OP_DISCONNECT, index);
=======
	mgmt_pending_foreach(MGMT_OP_UNPAIR_DEVICE, hdev, unpair_device_rsp,
			     hdev);
}

void mgmt_disconnect_failed(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 link_type, u8 addr_type, u8 status)
{
	u8 bdaddr_type = link_to_bdaddr(link_type, addr_type);
	struct mgmt_cp_disconnect *cp;
	struct mgmt_pending_cmd *cmd;

	mgmt_pending_foreach(MGMT_OP_UNPAIR_DEVICE, hdev, unpair_device_rsp,
			     hdev);

	cmd = pending_find(MGMT_OP_DISCONNECT, hdev);
>>>>>>> android-4.9
	if (!cmd)
		return;

	cp = cmd->param;

<<<<<<< HEAD
	err = cmd_status(cmd->sk, index, MGMT_OP_DISCONNECT, EIO);
=======
	if (bacmp(bdaddr, &cp->addr.bdaddr))
		return;

	if (cp->addr.type != bdaddr_type)
		return;
>>>>>>> android-4.9

	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);
<<<<<<< HEAD

	return err;
}

int mgmt_connect_failed(u16 index, bdaddr_t *bdaddr, u8 status)
{
	struct mgmt_ev_connect_failed ev;

	bacpy(&ev.bdaddr, bdaddr);
	ev.status = status;

	return mgmt_event(MGMT_EV_CONNECT_FAILED, index, &ev, sizeof(ev), NULL);
}

int mgmt_pin_code_request(u16 index, bdaddr_t *bdaddr)
{
	struct mgmt_ev_pin_code_request ev;

	BT_DBG("hci%u", index);

	bacpy(&ev.bdaddr, bdaddr);
	ev.secure = 0;

	return mgmt_event(MGMT_EV_PIN_CODE_REQUEST, index, &ev, sizeof(ev),
									NULL);
}

int mgmt_pin_code_reply_complete(u16 index, bdaddr_t *bdaddr, u8 status)
=======
}

void mgmt_connect_failed(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
			 u8 addr_type, u8 status)
{
	struct mgmt_ev_connect_failed ev;

	/* The connection is still in hci_conn_hash so test for 1
	 * instead of 0 to know if this is the last one.
	 */
	if (mgmt_powering_down(hdev) && hci_conn_count(hdev) == 1) {
		cancel_delayed_work(&hdev->power_off);
		queue_work(hdev->req_workqueue, &hdev->power_off.work);
	}

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(link_type, addr_type);
	ev.status = mgmt_status(status);

	mgmt_event(MGMT_EV_CONNECT_FAILED, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_pin_code_request(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 secure)
{
	struct mgmt_ev_pin_code_request ev;

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = BDADDR_BREDR;
	ev.secure = secure;

	mgmt_event(MGMT_EV_PIN_CODE_REQUEST, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_pin_code_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				  u8 status)
>>>>>>> android-4.9
{
	struct mgmt_pending_cmd *cmd;

<<<<<<< HEAD
	cmd = mgmt_pending_find(MGMT_OP_PIN_CODE_REPLY, index);
	if (!cmd)
		return -ENOENT;

	bacpy(&rp.bdaddr, bdaddr);
	rp.status = status;

	err = cmd_complete(cmd->sk, index, MGMT_OP_PIN_CODE_REPLY, &rp,
								sizeof(rp));
=======
	cmd = pending_find(MGMT_OP_PIN_CODE_REPLY, hdev);
	if (!cmd)
		return;
>>>>>>> android-4.9

	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);
}

<<<<<<< HEAD
int mgmt_pin_code_neg_reply_complete(u16 index, bdaddr_t *bdaddr, u8 status)
=======
void mgmt_pin_code_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				      u8 status)
>>>>>>> android-4.9
{
	struct mgmt_pending_cmd *cmd;

<<<<<<< HEAD
	cmd = mgmt_pending_find(MGMT_OP_PIN_CODE_NEG_REPLY, index);
	if (!cmd)
		return -ENOENT;

	bacpy(&rp.bdaddr, bdaddr);
	rp.status = status;

	err = cmd_complete(cmd->sk, index, MGMT_OP_PIN_CODE_NEG_REPLY, &rp,
								sizeof(rp));
=======
	cmd = pending_find(MGMT_OP_PIN_CODE_NEG_REPLY, hdev);
	if (!cmd)
		return;
>>>>>>> android-4.9

	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);
}

<<<<<<< HEAD
int mgmt_user_confirm_request(u16 index, u8 event,
					bdaddr_t *bdaddr, __le32 value)
=======
int mgmt_user_confirm_request(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type, u32 value,
			      u8 confirm_hint)
>>>>>>> android-4.9
{
	struct mgmt_ev_user_confirm_request ev;
	struct hci_conn *conn = NULL;
	struct hci_dev *hdev;
	u8 loc_cap, rem_cap, loc_mitm, rem_mitm;

<<<<<<< HEAD
	BT_DBG("hci%u", index);
=======
	BT_DBG("%s", hdev->name);

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(link_type, addr_type);
	ev.confirm_hint = confirm_hint;
	ev.value = cpu_to_le32(value);

	return mgmt_event(MGMT_EV_USER_CONFIRM_REQUEST, hdev, &ev, sizeof(ev),
			  NULL);
}

int mgmt_user_passkey_request(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type)
{
	struct mgmt_ev_user_passkey_request ev;

	BT_DBG("%s", hdev->name);

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(link_type, addr_type);

	return mgmt_event(MGMT_EV_USER_PASSKEY_REQUEST, hdev, &ev, sizeof(ev),
			  NULL);
}

static int user_pairing_resp_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				      u8 link_type, u8 addr_type, u8 status,
				      u8 opcode)
{
	struct mgmt_pending_cmd *cmd;

	cmd = pending_find(opcode, hdev);
	if (!cmd)
		return -ENOENT;

	cmd->cmd_complete(cmd, mgmt_status(status));
	mgmt_pending_remove(cmd);

	return 0;
}

int mgmt_user_confirm_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				     u8 link_type, u8 addr_type, u8 status)
{
	return user_pairing_resp_complete(hdev, bdaddr, link_type, addr_type,
					  status, MGMT_OP_USER_CONFIRM_REPLY);
}

int mgmt_user_confirm_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
					 u8 link_type, u8 addr_type, u8 status)
{
	return user_pairing_resp_complete(hdev, bdaddr, link_type, addr_type,
					  status,
					  MGMT_OP_USER_CONFIRM_NEG_REPLY);
}
>>>>>>> android-4.9

	hdev = hci_dev_get(index);

<<<<<<< HEAD
	if (!hdev)
		return -ENODEV;

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, bdaddr);

	ev.auto_confirm = 0;

	if (!conn || event != HCI_EV_USER_CONFIRM_REQUEST)
		goto no_auto_confirm;

	loc_cap = (conn->io_capability == 0x04) ? 0x01 : conn->io_capability;
	rem_cap = conn->remote_cap;
	loc_mitm = conn->auth_type & 0x01;
	rem_mitm = conn->remote_auth & 0x01;

	if ((conn->auth_type & HCI_AT_DEDICATED_BONDING) &&
			conn->auth_initiator && rem_cap == 0x03)
		ev.auto_confirm = 1;
	else if (loc_cap == 0x01 && (rem_cap == 0x00 || rem_cap == 0x03)) {
		if (!loc_mitm && !rem_mitm)
			value = 0;
		goto no_auto_confirm;
	}

	/* Show bonding dialog if neither side requires no bonding */
	if ((conn->auth_type > 0x01) && (conn->remote_auth > 0x01)) {
		if (!loc_mitm && !rem_mitm)
			value = 0;
		goto no_auto_confirm;
	}
=======
int mgmt_user_passkey_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
					 u8 link_type, u8 addr_type, u8 status)
{
	return user_pairing_resp_complete(hdev, bdaddr, link_type, addr_type,
					  status,
					  MGMT_OP_USER_PASSKEY_NEG_REPLY);
}

int mgmt_user_passkey_notify(struct hci_dev *hdev, bdaddr_t *bdaddr,
			     u8 link_type, u8 addr_type, u32 passkey,
			     u8 entered)
{
	struct mgmt_ev_passkey_notify ev;

	BT_DBG("%s", hdev->name);

	bacpy(&ev.addr.bdaddr, bdaddr);
	ev.addr.type = link_to_bdaddr(link_type, addr_type);
	ev.passkey = __cpu_to_le32(passkey);
	ev.entered = entered;

	return mgmt_event(MGMT_EV_PASSKEY_NOTIFY, hdev, &ev, sizeof(ev), NULL);
}

void mgmt_auth_failed(struct hci_conn *conn, u8 hci_status)
{
	struct mgmt_ev_auth_failed ev;
	struct mgmt_pending_cmd *cmd;
	u8 status = mgmt_status(hci_status);

	bacpy(&ev.addr.bdaddr, &conn->dst);
	ev.addr.type = link_to_bdaddr(conn->type, conn->dst_type);
	ev.status = status;

	cmd = find_pairing(conn);

	mgmt_event(MGMT_EV_AUTH_FAILED, conn->hdev, &ev, sizeof(ev),
		    cmd ? cmd->sk : NULL);

	if (cmd) {
		cmd->cmd_complete(cmd, status);
		mgmt_pending_remove(cmd);
	}
}

void mgmt_auth_enable_complete(struct hci_dev *hdev, u8 status)
{
	struct cmd_lookup match = { NULL, hdev };
	bool changed;

	if (status) {
		u8 mgmt_err = mgmt_status(status);
		mgmt_pending_foreach(MGMT_OP_SET_LINK_SECURITY, hdev,
				     cmd_status_rsp, &mgmt_err);
		return;
	}

	if (test_bit(HCI_AUTH, &hdev->flags))
		changed = !hci_dev_test_and_set_flag(hdev, HCI_LINK_SECURITY);
	else
		changed = hci_dev_test_and_clear_flag(hdev, HCI_LINK_SECURITY);
>>>>>>> android-4.9

	if ((!loc_mitm || rem_cap == 0x03) && (!rem_mitm || loc_cap == 0x03))
		ev.auto_confirm = 1;

<<<<<<< HEAD
no_auto_confirm:
	bacpy(&ev.bdaddr, bdaddr);
	ev.event = event;
	put_unaligned_le32(value, &ev.value);

	hci_dev_put(hdev);

	return mgmt_event(MGMT_EV_USER_CONFIRM_REQUEST, index, &ev, sizeof(ev),
									NULL);
}

int mgmt_user_passkey_request(u16 index, bdaddr_t *bdaddr)
{
	struct mgmt_ev_user_passkey_request ev;
=======
	if (changed)
		new_settings(hdev, match.sk);

	if (match.sk)
		sock_put(match.sk);
}

static void clear_eir(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_write_eir cp;

	if (!lmp_ext_inq_capable(hdev))
		return;
>>>>>>> android-4.9

	BT_DBG("hci%u", index);

	bacpy(&ev.bdaddr, bdaddr);

<<<<<<< HEAD
	return mgmt_event(MGMT_EV_USER_PASSKEY_REQUEST, index, &ev, sizeof(ev),
									NULL);
}

static int confirm_reply_complete(u16 index, bdaddr_t *bdaddr, u8 status,
								u8 opcode)
{
	struct pending_cmd *cmd;
	struct mgmt_rp_user_confirm_reply rp;
	int err;

	cmd = mgmt_pending_find(opcode, index);
	if (!cmd)
		return -ENOENT;
=======
	hci_req_add(req, HCI_OP_WRITE_EIR, sizeof(cp), &cp);
}

void mgmt_ssp_enable_complete(struct hci_dev *hdev, u8 enable, u8 status)
{
	struct cmd_lookup match = { NULL, hdev };
	struct hci_request req;
	bool changed = false;

	if (status) {
		u8 mgmt_err = mgmt_status(status);

		if (enable && hci_dev_test_and_clear_flag(hdev,
							  HCI_SSP_ENABLED)) {
			hci_dev_clear_flag(hdev, HCI_HS_ENABLED);
			new_settings(hdev, NULL);
		}

		mgmt_pending_foreach(MGMT_OP_SET_SSP, hdev, cmd_status_rsp,
				     &mgmt_err);
		return;
	}

	if (enable) {
		changed = !hci_dev_test_and_set_flag(hdev, HCI_SSP_ENABLED);
	} else {
		changed = hci_dev_test_and_clear_flag(hdev, HCI_SSP_ENABLED);
		if (!changed)
			changed = hci_dev_test_and_clear_flag(hdev,
							      HCI_HS_ENABLED);
		else
			hci_dev_clear_flag(hdev, HCI_HS_ENABLED);
	}

	mgmt_pending_foreach(MGMT_OP_SET_SSP, hdev, settings_rsp, &match);

	if (changed)
		new_settings(hdev, match.sk);
>>>>>>> android-4.9

	bacpy(&rp.bdaddr, bdaddr);
	rp.status = status;
	err = cmd_complete(cmd->sk, index, opcode, &rp, sizeof(rp));

<<<<<<< HEAD
	mgmt_pending_remove(cmd);
=======
	hci_req_init(&req, hdev);
>>>>>>> android-4.9

	if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED)) {
		if (hci_dev_test_flag(hdev, HCI_USE_DEBUG_KEYS))
			hci_req_add(&req, HCI_OP_WRITE_SSP_DEBUG_MODE,
				    sizeof(enable), &enable);
		__hci_req_update_eir(&req);
	} else {
		clear_eir(&req);
	}

	hci_req_run(&req, NULL);
}

<<<<<<< HEAD
int mgmt_user_confirm_reply_complete(u16 index, bdaddr_t *bdaddr, u8 status)
{
	return confirm_reply_complete(index, bdaddr, status,
						MGMT_OP_USER_CONFIRM_REPLY);
}

int mgmt_user_confirm_neg_reply_complete(u16 index, bdaddr_t *bdaddr, u8 status)
{
	return confirm_reply_complete(index, bdaddr, status,
					MGMT_OP_USER_CONFIRM_NEG_REPLY);
}

int mgmt_auth_failed(u16 index, bdaddr_t *bdaddr, u8 status)
{
	struct mgmt_ev_auth_failed ev;

	bacpy(&ev.bdaddr, bdaddr);
	ev.status = status;

	return mgmt_event(MGMT_EV_AUTH_FAILED, index, &ev, sizeof(ev), NULL);
}

int mgmt_set_local_name_complete(u16 index, u8 *name, u8 status)
{
	struct pending_cmd *cmd;
	struct hci_dev *hdev;
	struct mgmt_cp_set_local_name ev;
	int err;
=======
static void sk_lookup(struct mgmt_pending_cmd *cmd, void *data)
{
	struct cmd_lookup *match = data;

	if (match->sk == NULL) {
		match->sk = cmd->sk;
		sock_hold(match->sk);
	}
}

void mgmt_set_class_of_dev_complete(struct hci_dev *hdev, u8 *dev_class,
				    u8 status)
{
	struct cmd_lookup match = { NULL, hdev, mgmt_status(status) };

	mgmt_pending_foreach(MGMT_OP_SET_DEV_CLASS, hdev, sk_lookup, &match);
	mgmt_pending_foreach(MGMT_OP_ADD_UUID, hdev, sk_lookup, &match);
	mgmt_pending_foreach(MGMT_OP_REMOVE_UUID, hdev, sk_lookup, &match);

	if (!status) {
		mgmt_limited_event(MGMT_EV_CLASS_OF_DEV_CHANGED, hdev, dev_class,
				   3, HCI_MGMT_DEV_CLASS_EVENTS, NULL);
		ext_info_changed(hdev, NULL);
	}

	if (match.sk)
		sock_put(match.sk);
}

void mgmt_set_local_name_complete(struct hci_dev *hdev, u8 *name, u8 status)
{
	struct mgmt_cp_set_local_name ev;
	struct mgmt_pending_cmd *cmd;

	if (status)
		return;
>>>>>>> android-4.9

	memset(&ev, 0, sizeof(ev));
	memcpy(ev.name, name, HCI_MAX_NAME_LENGTH);

<<<<<<< HEAD
	cmd = mgmt_pending_find(MGMT_OP_SET_LOCAL_NAME, index);
	if (!cmd)
		goto send_event;

	if (status) {
		err = cmd_status(cmd->sk, index, MGMT_OP_SET_LOCAL_NAME, EIO);
		goto failed;
	}

	hdev = hci_dev_get(index);
	if (hdev) {
		update_eir(hdev);
		hci_dev_put(hdev);
	}

	err = cmd_complete(cmd->sk, index, MGMT_OP_SET_LOCAL_NAME, &ev,
								sizeof(ev));
	if (err < 0)
		goto failed;

send_event:
	err = mgmt_event(MGMT_EV_LOCAL_NAME_CHANGED, index, &ev, sizeof(ev),
							cmd ? cmd->sk : NULL);
=======
	cmd = pending_find(MGMT_OP_SET_LOCAL_NAME, hdev);
	if (!cmd) {
		memcpy(hdev->dev_name, name, sizeof(hdev->dev_name));

		/* If this is a HCI command related to powering on the
		 * HCI dev don't send any mgmt signals.
		 */
		if (pending_find(MGMT_OP_SET_POWERED, hdev))
			return;
	}

	mgmt_limited_event(MGMT_EV_LOCAL_NAME_CHANGED, hdev, &ev, sizeof(ev),
			   HCI_MGMT_LOCAL_NAME_EVENTS, cmd ? cmd->sk : NULL);
	ext_info_changed(hdev, cmd ? cmd->sk : NULL);
}

static inline bool has_uuid(u8 *uuid, u16 uuid_count, u8 (*uuids)[16])
{
	int i;

	for (i = 0; i < uuid_count; i++) {
		if (!memcmp(uuid, uuids[i], 16))
			return true;
	}
>>>>>>> android-4.9

	return false;
}

<<<<<<< HEAD
int mgmt_read_local_oob_data_reply_complete(u16 index, u8 *hash, u8 *randomizer,
								u8 status)
=======
static bool eir_has_uuids(u8 *eir, u16 eir_len, u16 uuid_count, u8 (*uuids)[16])
>>>>>>> android-4.9
{
	u16 parsed = 0;

<<<<<<< HEAD
	BT_DBG("hci%u status %u", index, status);

	cmd = mgmt_pending_find(MGMT_OP_READ_LOCAL_OOB_DATA, index);
	if (!cmd)
		return -ENOENT;

	if (status) {
		err = cmd_status(cmd->sk, index, MGMT_OP_READ_LOCAL_OOB_DATA,
									EIO);
	} else {
		struct mgmt_rp_read_local_oob_data rp;
=======
	while (parsed < eir_len) {
		u8 field_len = eir[0];
		u8 uuid[16];
		int i;

		if (field_len == 0)
			break;

		if (eir_len - parsed < field_len + 1)
			break;
>>>>>>> android-4.9

		switch (eir[1]) {
		case EIR_UUID16_ALL:
		case EIR_UUID16_SOME:
			for (i = 0; i + 3 <= field_len; i += 2) {
				memcpy(uuid, bluetooth_base_uuid, 16);
				uuid[13] = eir[i + 3];
				uuid[12] = eir[i + 2];
				if (has_uuid(uuid, uuid_count, uuids))
					return true;
			}
			break;
		case EIR_UUID32_ALL:
		case EIR_UUID32_SOME:
			for (i = 0; i + 5 <= field_len; i += 4) {
				memcpy(uuid, bluetooth_base_uuid, 16);
				uuid[15] = eir[i + 5];
				uuid[14] = eir[i + 4];
				uuid[13] = eir[i + 3];
				uuid[12] = eir[i + 2];
				if (has_uuid(uuid, uuid_count, uuids))
					return true;
			}
			break;
		case EIR_UUID128_ALL:
		case EIR_UUID128_SOME:
			for (i = 0; i + 17 <= field_len; i += 16) {
				memcpy(uuid, eir + i + 2, 16);
				if (has_uuid(uuid, uuid_count, uuids))
					return true;
			}
			break;
		}

<<<<<<< HEAD
		err = cmd_complete(cmd->sk, index, MGMT_OP_READ_LOCAL_OOB_DATA,
							&rp, sizeof(rp));
=======
		parsed += field_len + 1;
		eir += field_len + 1;
>>>>>>> android-4.9
	}

	return false;
}

<<<<<<< HEAD
void mgmt_read_rssi_complete(u16 index, s8 rssi, bdaddr_t *bdaddr,
		u16 handle, u8 status)
{
	struct mgmt_ev_rssi_update ev;
	struct hci_conn *conn;
	struct hci_dev *hdev;

	if (status)
		return;

	hdev = hci_dev_get(index);
	conn = hci_conn_hash_lookup_handle(hdev, handle);

	if (!conn)
		return;

	BT_DBG("rssi_update_thresh_exceed : %d ",
		   conn->rssi_update_thresh_exceed);
	BT_DBG("RSSI Threshold : %d , recvd RSSI : %d ",
			conn->rssi_threshold, rssi);

	if (conn->rssi_update_thresh_exceed == 1) {
		BT_DBG("rssi_update_thresh_exceed == 1");
		if (rssi > conn->rssi_threshold) {
			memset(&ev, 0, sizeof(ev));
			bacpy(&ev.bdaddr, bdaddr);
			ev.rssi = rssi;
			mgmt_event(MGMT_EV_RSSI_UPDATE, index, &ev,
				sizeof(ev), NULL);
		} else {
			hci_conn_set_rssi_reporter(conn, conn->rssi_threshold,
				conn->rssi_update_interval,
				conn->rssi_update_thresh_exceed);
		}
	} else {
		BT_DBG("rssi_update_thresh_exceed == 0");
		if (rssi < conn->rssi_threshold) {
			memset(&ev, 0, sizeof(ev));
			bacpy(&ev.bdaddr, bdaddr);
			ev.rssi = rssi;
			mgmt_event(MGMT_EV_RSSI_UPDATE, index, &ev,
				sizeof(ev), NULL);
		} else {
			hci_conn_set_rssi_reporter(conn, conn->rssi_threshold,
				conn->rssi_update_interval,
				conn->rssi_update_thresh_exceed);
		}
	}
}


int mgmt_device_found(u16 index, bdaddr_t *bdaddr, u8 type, u8 le,
			u8 *dev_class, s8 rssi, u8 eir_len, u8 *eir)
{
	struct mgmt_ev_device_found ev;
	struct hci_dev *hdev;
	int err;

	BT_DBG("le: %d", le);

	memset(&ev, 0, sizeof(ev));

	bacpy(&ev.bdaddr, bdaddr);
	ev.rssi = rssi;
	ev.type = type;
	ev.le = le;

	if (dev_class)
		memcpy(ev.dev_class, dev_class, sizeof(ev.dev_class));

	if (eir && eir_len)
		memcpy(ev.eir, eir, eir_len);

	err = mgmt_event(MGMT_EV_DEVICE_FOUND, index, &ev, sizeof(ev), NULL);
=======
static void restart_le_scan(struct hci_dev *hdev)
{
	/* If controller is not scanning we are done. */
	if (!hci_dev_test_flag(hdev, HCI_LE_SCAN))
		return;

	if (time_after(jiffies + DISCOV_LE_RESTART_DELAY,
		       hdev->discovery.scan_start +
		       hdev->discovery.scan_duration))
		return;

	queue_delayed_work(hdev->req_workqueue, &hdev->le_scan_restart,
			   DISCOV_LE_RESTART_DELAY);
}

static bool is_filter_match(struct hci_dev *hdev, s8 rssi, u8 *eir,
			    u16 eir_len, u8 *scan_rsp, u8 scan_rsp_len)
{
	/* If a RSSI threshold has been specified, and
	 * HCI_QUIRK_STRICT_DUPLICATE_FILTER is not set, then all results with
	 * a RSSI smaller than the RSSI threshold will be dropped. If the quirk
	 * is set, let it through for further processing, as we might need to
	 * restart the scan.
	 *
	 * For BR/EDR devices (pre 1.2) providing no RSSI during inquiry,
	 * the results are also dropped.
	 */
	if (hdev->discovery.rssi != HCI_RSSI_INVALID &&
	    (rssi == HCI_RSSI_INVALID ||
	    (rssi < hdev->discovery.rssi &&
	     !test_bit(HCI_QUIRK_STRICT_DUPLICATE_FILTER, &hdev->quirks))))
		return  false;

	if (hdev->discovery.uuid_count != 0) {
		/* If a list of UUIDs is provided in filter, results with no
		 * matching UUID should be dropped.
		 */
		if (!eir_has_uuids(eir, eir_len, hdev->discovery.uuid_count,
				   hdev->discovery.uuids) &&
		    !eir_has_uuids(scan_rsp, scan_rsp_len,
				   hdev->discovery.uuid_count,
				   hdev->discovery.uuids))
			return false;
	}

	/* If duplicate filtering does not report RSSI changes, then restart
	 * scanning to ensure updated result with updated RSSI values.
	 */
	if (test_bit(HCI_QUIRK_STRICT_DUPLICATE_FILTER, &hdev->quirks)) {
		restart_le_scan(hdev);

		/* Validate RSSI value against the RSSI threshold once more. */
		if (hdev->discovery.rssi != HCI_RSSI_INVALID &&
		    rssi < hdev->discovery.rssi)
			return false;
	}

	return true;
}

void mgmt_device_found(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
		       u8 addr_type, u8 *dev_class, s8 rssi, u32 flags,
		       u8 *eir, u16 eir_len, u8 *scan_rsp, u8 scan_rsp_len)
{
	char buf[512];
	struct mgmt_ev_device_found *ev = (void *)buf;
	size_t ev_size;

	/* Don't send events for a non-kernel initiated discovery. With
	 * LE one exception is if we have pend_le_reports > 0 in which
	 * case we're doing passive scanning and want these events.
	 */
	if (!hci_discovery_active(hdev)) {
		if (link_type == ACL_LINK)
			return;
		if (link_type == LE_LINK && list_empty(&hdev->pend_le_reports))
			return;
	}

	if (hdev->discovery.result_filtering) {
		/* We are using service discovery */
		if (!is_filter_match(hdev, rssi, eir, eir_len, scan_rsp,
				     scan_rsp_len))
			return;
	}

	if (hdev->discovery.limited) {
		/* Check for limited discoverable bit */
		if (dev_class) {
			if (!(dev_class[1] & 0x20))
				return;
		} else {
			u8 *flags = eir_get_data(eir, eir_len, EIR_FLAGS, NULL);
			if (!flags || !(flags[0] & LE_AD_LIMITED))
				return;
		}
	}

	/* Make sure that the buffer is big enough. The 5 extra bytes
	 * are for the potential CoD field.
	 */
	if (sizeof(*ev) + eir_len + scan_rsp_len + 5 > sizeof(buf))
		return;

	memset(buf, 0, sizeof(buf));

	/* In case of device discovery with BR/EDR devices (pre 1.2), the
	 * RSSI value was reported as 0 when not available. This behavior
	 * is kept when using device discovery. This is required for full
	 * backwards compatibility with the API.
	 *
	 * However when using service discovery, the value 127 will be
	 * returned when the RSSI is not available.
	 */
	if (rssi == HCI_RSSI_INVALID && !hdev->discovery.report_invalid_rssi &&
	    link_type == ACL_LINK)
		rssi = 0;

	bacpy(&ev->addr.bdaddr, bdaddr);
	ev->addr.type = link_to_bdaddr(link_type, addr_type);
	ev->rssi = rssi;
	ev->flags = cpu_to_le32(flags);

	if (eir_len > 0)
		/* Copy EIR or advertising data into event */
		memcpy(ev->eir, eir, eir_len);

	if (dev_class && !eir_get_data(ev->eir, eir_len, EIR_CLASS_OF_DEV,
				       NULL))
		eir_len = eir_append_data(ev->eir, eir_len, EIR_CLASS_OF_DEV,
					  dev_class, 3);

	if (scan_rsp_len > 0)
		/* Append scan response data to event */
		memcpy(ev->eir + eir_len, scan_rsp, scan_rsp_len);

	ev->eir_len = cpu_to_le16(eir_len + scan_rsp_len);
	ev_size = sizeof(*ev) + eir_len + scan_rsp_len;

	mgmt_event(MGMT_EV_DEVICE_FOUND, hdev, ev, ev_size, NULL);
}

void mgmt_remote_name(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
		      u8 addr_type, s8 rssi, u8 *name, u8 name_len)
{
	struct mgmt_ev_device_found *ev;
	char buf[sizeof(*ev) + HCI_MAX_NAME_LENGTH + 2];
	u16 eir_len;
>>>>>>> android-4.9

	if (err < 0)
		return err;

	hdev = hci_dev_get(index);

<<<<<<< HEAD
	if (!hdev)
		return 0;
=======
	bacpy(&ev->addr.bdaddr, bdaddr);
	ev->addr.type = link_to_bdaddr(link_type, addr_type);
	ev->rssi = rssi;
>>>>>>> android-4.9

	if (hdev->disco_state == SCAN_IDLE)
		goto done;

<<<<<<< HEAD
	hdev->disco_int_count++;

	if (hdev->disco_int_count >= hdev->disco_int_phase) {
		/* Inquiry scan for General Discovery LAP */
		struct hci_cp_inquiry cp = {{0x33, 0x8b, 0x9e}, 4, 0};
		struct hci_cp_le_set_scan_enable le_cp = {0, 0};

		hdev->disco_int_phase *= 2;
		hdev->disco_int_count = 0;
		if (hdev->disco_state == SCAN_LE) {
			/* cancel LE scan */
			hci_send_cmd(hdev, HCI_OP_LE_SET_SCAN_ENABLE,
					sizeof(le_cp), &le_cp);
			/* start BR scan */
			cp.num_rsp = (u8) hdev->disco_int_phase;
			hci_send_cmd(hdev, HCI_OP_INQUIRY,
					sizeof(cp), &cp);
			hdev->disco_state = SCAN_BR;
			del_timer_sync(&hdev->disco_le_timer);
		}
	}

done:
	hci_dev_put(hdev);
	return 0;
}


int mgmt_remote_name(u16 index, bdaddr_t *bdaddr, u8 status, u8 *name)
{
	struct mgmt_ev_remote_name ev;

	memset(&ev, 0, sizeof(ev));

	bacpy(&ev.bdaddr, bdaddr);
	ev.status = status;
	memcpy(ev.name, name, HCI_MAX_NAME_LENGTH);

	return mgmt_event(MGMT_EV_REMOTE_NAME, index, &ev, sizeof(ev), NULL);
}

int mgmt_encrypt_change(u16 index, bdaddr_t *bdaddr, u8 status)
{
	struct mgmt_ev_encrypt_change ev;

	BT_DBG("hci%u", index);

	bacpy(&ev.bdaddr, bdaddr);
	ev.status = status;

	return mgmt_event(MGMT_EV_ENCRYPT_CHANGE, index, &ev, sizeof(ev),
									NULL);
}

int mgmt_remote_class(u16 index, bdaddr_t *bdaddr, u8 dev_class[3])
{
	struct mgmt_ev_remote_class ev;
=======
	ev->eir_len = cpu_to_le16(eir_len);

	mgmt_event(MGMT_EV_DEVICE_FOUND, hdev, ev, sizeof(*ev) + eir_len, NULL);
}

void mgmt_discovering(struct hci_dev *hdev, u8 discovering)
{
	struct mgmt_ev_discovering ev;

	BT_DBG("%s discovering %u", hdev->name, discovering);
>>>>>>> android-4.9

	memset(&ev, 0, sizeof(ev));

<<<<<<< HEAD
	bacpy(&ev.bdaddr, bdaddr);
	memcpy(ev.dev_class, dev_class, 3);

	return mgmt_event(MGMT_EV_REMOTE_CLASS, index, &ev, sizeof(ev), NULL);
}

int mgmt_remote_version(u16 index, bdaddr_t *bdaddr, u8 ver, u16 mnf,
							u16 sub_ver)
{
	struct mgmt_ev_remote_version ev;

	memset(&ev, 0, sizeof(ev));

	bacpy(&ev.bdaddr, bdaddr);
	ev.lmp_ver = ver;
	ev.manufacturer = mnf;
	ev.lmp_subver = sub_ver;

	return mgmt_event(MGMT_EV_REMOTE_VERSION, index, &ev, sizeof(ev), NULL);
}

int mgmt_remote_features(u16 index, bdaddr_t *bdaddr, u8 features[8])
{
	struct mgmt_ev_remote_features ev;

	memset(&ev, 0, sizeof(ev));

	bacpy(&ev.bdaddr, bdaddr);
	memcpy(ev.features, features, sizeof(ev.features));

	return mgmt_event(MGMT_EV_REMOTE_FEATURES, index, &ev, sizeof(ev),
									NULL);
=======
	mgmt_event(MGMT_EV_DISCOVERING, hdev, &ev, sizeof(ev), NULL);
}

static struct hci_mgmt_chan chan = {
	.channel	= HCI_CHANNEL_CONTROL,
	.handler_count	= ARRAY_SIZE(mgmt_handlers),
	.handlers	= mgmt_handlers,
	.hdev_init	= mgmt_init_hdev,
};

int mgmt_init(void)
{
	return hci_mgmt_chan_register(&chan);
}

void mgmt_exit(void)
{
	hci_mgmt_chan_unregister(&chan);
>>>>>>> android-4.9
}
