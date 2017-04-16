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

/* Bluetooth HCI core. */

<<<<<<< HEAD
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/kmod.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/notifier.h>
=======
#include <linux/export.h>
#include <linux/idr.h>
>>>>>>> android-4.9
#include <linux/rfkill.h>
#include <linux/debugfs.h>
#include <linux/crypto.h>
<<<<<<< HEAD
#include <net/sock.h>

#include <asm/system.h>
#include <linux/uaccess.h>
=======
>>>>>>> android-4.9
#include <asm/unaligned.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/l2cap.h>
#include <net/bluetooth/mgmt.h>

#include "hci_request.h"
#include "hci_debugfs.h"
#include "smp.h"
#include "leds.h"

static void hci_cmd_task(unsigned long arg);
static void hci_rx_task(unsigned long arg);
static void hci_tx_task(unsigned long arg);

static DEFINE_RWLOCK(hci_task_lock);

static bool enable_smp = 1;

/* HCI device list */
LIST_HEAD(hci_dev_list);
DEFINE_RWLOCK(hci_dev_list_lock);

/* HCI callback list */
LIST_HEAD(hci_cb_list);
DEFINE_MUTEX(hci_cb_list_lock);

/* HCI ID Numbering */
static DEFINE_IDA(hci_index_ida);

<<<<<<< HEAD
/* AMP Manager event callbacks */
LIST_HEAD(amp_mgr_cb_list);
DEFINE_RWLOCK(amp_mgr_cb_list_lock);

/* HCI protocols */
#define HCI_MAX_PROTO	2
struct hci_proto *hci_proto[HCI_MAX_PROTO];

/* HCI notifiers list */
static ATOMIC_NOTIFIER_HEAD(hci_notifier);

/* ---- HCI notifications ---- */

int hci_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&hci_notifier, nb);
}

int hci_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&hci_notifier, nb);
}

static void hci_notify(struct hci_dev *hdev, int event)
{
	atomic_notifier_call_chain(&hci_notifier, event, hdev);
}
=======
/* ---- HCI debugfs entries ---- */

static ssize_t dut_mode_read(struct file *file, char __user *user_buf,
			     size_t count, loff_t *ppos)
{
	struct hci_dev *hdev = file->private_data;
	char buf[3];
>>>>>>> android-4.9

	buf[0] = hci_dev_test_flag(hdev, HCI_DUT_MODE) ? 'Y' : 'N';
	buf[1] = '\n';
	buf[2] = '\0';
	return simple_read_from_buffer(user_buf, count, ppos, buf, 2);
}

static ssize_t dut_mode_write(struct file *file, const char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	struct hci_dev *hdev = file->private_data;
	struct sk_buff *skb;
	char buf[32];
	size_t buf_size = min(count, (sizeof(buf)-1));
	bool enable;

<<<<<<< HEAD
	/* If this is the init phase check if the completed command matches
	 * the last init command, and if not just return.
	 */
	if (test_bit(HCI_INIT, &hdev->flags) && hdev->init_last_cmd != cmd)
		return;
=======
	if (!test_bit(HCI_UP, &hdev->flags))
		return -ENETDOWN;

	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;

	buf[buf_size] = '\0';
	if (strtobool(buf, &enable))
		return -EINVAL;

	if (enable == hci_dev_test_flag(hdev, HCI_DUT_MODE))
		return -EALREADY;

	hci_req_sync_lock(hdev);
	if (enable)
		skb = __hci_cmd_sync(hdev, HCI_OP_ENABLE_DUT_MODE, 0, NULL,
				     HCI_CMD_TIMEOUT);
	else
		skb = __hci_cmd_sync(hdev, HCI_OP_RESET, 0, NULL,
				     HCI_CMD_TIMEOUT);
	hci_req_sync_unlock(hdev);
>>>>>>> android-4.9

	if (IS_ERR(skb))
		return PTR_ERR(skb);

	kfree_skb(skb);

	hci_dev_change_flag(hdev, HCI_DUT_MODE);

	return count;
}

static const struct file_operations dut_mode_fops = {
	.open		= simple_open,
	.read		= dut_mode_read,
	.write		= dut_mode_write,
	.llseek		= default_llseek,
};

static ssize_t vendor_diag_read(struct file *file, char __user *user_buf,
				size_t count, loff_t *ppos)
{
	struct hci_dev *hdev = file->private_data;
	char buf[3];

	buf[0] = hci_dev_test_flag(hdev, HCI_VENDOR_DIAG) ? 'Y' : 'N';
	buf[1] = '\n';
	buf[2] = '\0';
	return simple_read_from_buffer(user_buf, count, ppos, buf, 2);
}

static ssize_t vendor_diag_write(struct file *file, const char __user *user_buf,
				 size_t count, loff_t *ppos)
{
	struct hci_dev *hdev = file->private_data;
	char buf[32];
	size_t buf_size = min(count, (sizeof(buf)-1));
	bool enable;
	int err;

	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;

	buf[buf_size] = '\0';
	if (strtobool(buf, &enable))
		return -EINVAL;

	/* When the diagnostic flags are not persistent and the transport
	 * is not active, then there is no need for the vendor callback.
	 *
	 * Instead just store the desired value. If needed the setting
	 * will be programmed when the controller gets powered on.
	 */
	if (test_bit(HCI_QUIRK_NON_PERSISTENT_DIAG, &hdev->quirks) &&
	    !test_bit(HCI_RUNNING, &hdev->flags))
		goto done;

	hci_req_sync_lock(hdev);
	err = hdev->set_diag(hdev, enable);
	hci_req_sync_unlock(hdev);

	if (err < 0)
		return err;

done:
	if (enable)
		hci_dev_set_flag(hdev, HCI_VENDOR_DIAG);
	else
		hci_dev_clear_flag(hdev, HCI_VENDOR_DIAG);

<<<<<<< HEAD
	switch (hdev->req_status) {
	case HCI_REQ_DONE:
		err = -bt_err(hdev->req_result);
		break;
=======
	return count;
}
>>>>>>> android-4.9

static const struct file_operations vendor_diag_fops = {
	.open		= simple_open,
	.read		= vendor_diag_read,
	.write		= vendor_diag_write,
	.llseek		= default_llseek,
};

static void hci_debugfs_create_basic(struct hci_dev *hdev)
{
	debugfs_create_file("dut_mode", 0644, hdev->debugfs, hdev,
			    &dut_mode_fops);

	if (hdev->set_diag)
		debugfs_create_file("vendor_diag", 0644, hdev->debugfs, hdev,
				    &vendor_diag_fops);
}

static int hci_reset_req(struct hci_request *req, unsigned long opt)
{
	BT_DBG("%s %ld", req->hdev->name, opt);

	/* Reset device */
	set_bit(HCI_RESET, &req->hdev->flags);
	hci_req_add(req, HCI_OP_RESET, 0, NULL);
	return 0;
}

static void bredr_init(struct hci_request *req)
{
	req->hdev->flow_ctl_mode = HCI_FLOW_CTL_MODE_PACKET_BASED;

	/* Read Local Supported Features */
	hci_req_add(req, HCI_OP_READ_LOCAL_FEATURES, 0, NULL);

	/* Read Local Version */
	hci_req_add(req, HCI_OP_READ_LOCAL_VERSION, 0, NULL);

	/* Read BD Address */
	hci_req_add(req, HCI_OP_READ_BD_ADDR, 0, NULL);
}

static void amp_init1(struct hci_request *req)
{
	req->hdev->flow_ctl_mode = HCI_FLOW_CTL_MODE_BLOCK_BASED;

<<<<<<< HEAD
	/* Reset device */
	set_bit(HCI_RESET, &hdev->flags);
	memset(&hdev->features, 0, sizeof(hdev->features));
	hci_send_cmd(hdev, HCI_OP_RESET, 0, NULL);
}

static void hci_init_req(struct hci_dev *hdev, unsigned long opt)
{
	struct hci_cp_delete_stored_link_key cp;
	struct sk_buff *skb;
	__le16 param;
	__u8 flt_type;

	BT_DBG("%s %ld", hdev->name, opt);

	/* Driver initialization */

	/* Special commands */
	while ((skb = skb_dequeue(&hdev->driver_init))) {
		bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
		skb->dev = (void *) hdev;

		skb_queue_tail(&hdev->cmd_q, skb);
		tasklet_schedule(&hdev->cmd_task);
	}
	skb_queue_purge(&hdev->driver_init);
=======
	/* Read Local Version */
	hci_req_add(req, HCI_OP_READ_LOCAL_VERSION, 0, NULL);

	/* Read Local Supported Commands */
	hci_req_add(req, HCI_OP_READ_LOCAL_COMMANDS, 0, NULL);

	/* Read Local AMP Info */
	hci_req_add(req, HCI_OP_READ_LOCAL_AMP_INFO, 0, NULL);

	/* Read Data Blk size */
	hci_req_add(req, HCI_OP_READ_DATA_BLOCK_SIZE, 0, NULL);

	/* Read Flow Control Mode */
	hci_req_add(req, HCI_OP_READ_FLOW_CONTROL_MODE, 0, NULL);

	/* Read Location Data */
	hci_req_add(req, HCI_OP_READ_LOCATION_DATA, 0, NULL);
}

static int amp_init2(struct hci_request *req)
{
	/* Read Local Supported Features. Not all AMP controllers
	 * support this so it's placed conditionally in the second
	 * stage init.
	 */
	if (req->hdev->commands[14] & 0x20)
		hci_req_add(req, HCI_OP_READ_LOCAL_FEATURES, 0, NULL);

	return 0;
}
>>>>>>> android-4.9

static int hci_init1_req(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	BT_DBG("%s %ld", hdev->name, opt);

	/* Reset */
<<<<<<< HEAD
	if (!test_bit(HCI_QUIRK_NO_RESET, &hdev->quirks)) {
			set_bit(HCI_RESET, &hdev->flags);
			hci_send_cmd(hdev, HCI_OP_RESET, 0, NULL);
	}

	/* Read Local Version */
	hci_send_cmd(hdev, HCI_OP_READ_LOCAL_VERSION, 0, NULL);


	/* Set default HCI Flow Control Mode */
	if (hdev->dev_type == HCI_BREDR)
		hdev->flow_ctl_mode = HCI_PACKET_BASED_FLOW_CTL_MODE;
	else
		hdev->flow_ctl_mode = HCI_BLOCK_BASED_FLOW_CTL_MODE;

	/* Read HCI Flow Control Mode */
	hci_send_cmd(hdev, HCI_OP_READ_FLOW_CONTROL_MODE, 0, NULL);

	/* Read Buffer Size (ACL mtu, max pkt, etc.) */
	hci_send_cmd(hdev, HCI_OP_READ_BUFFER_SIZE, 0, NULL);

	/* Read Data Block Size (ACL mtu, max pkt, etc.) */
	hci_send_cmd(hdev, HCI_OP_READ_DATA_BLOCK_SIZE, 0, NULL);

#if 0
	/* Host buffer size */
	{
		struct hci_cp_host_buffer_size cp;
		cp.acl_mtu = cpu_to_le16(HCI_MAX_ACL_SIZE);
		cp.sco_mtu = HCI_MAX_SCO_SIZE;
		cp.acl_max_pkt = cpu_to_le16(0xffff);
		cp.sco_max_pkt = cpu_to_le16(0xffff);
		hci_send_cmd(hdev, HCI_OP_HOST_BUFFER_SIZE, sizeof(cp), &cp);
	}
#endif

	if (hdev->dev_type == HCI_BREDR) {
		/* BR-EDR initialization */

		/* Read Local Supported Features */
		hci_send_cmd(hdev, HCI_OP_READ_LOCAL_FEATURES, 0, NULL);

		/* Read BD Address */
		hci_send_cmd(hdev, HCI_OP_READ_BD_ADDR, 0, NULL);

		/* Read Class of Device */
		hci_send_cmd(hdev, HCI_OP_READ_CLASS_OF_DEV, 0, NULL);

		/* Read Local Name */
		hci_send_cmd(hdev, HCI_OP_READ_LOCAL_NAME, 0, NULL);

		/* Read Voice Setting */
		hci_send_cmd(hdev, HCI_OP_READ_VOICE_SETTING, 0, NULL);

		/* Optional initialization */
		/* Clear Event Filters */
		flt_type = HCI_FLT_CLEAR_ALL;
		hci_send_cmd(hdev, HCI_OP_SET_EVENT_FLT, 1, &flt_type);

		/* Connection accept timeout ~20 secs */
		param = cpu_to_le16(0x7d00);
		hci_send_cmd(hdev, HCI_OP_WRITE_CA_TIMEOUT, 2, &param);

		bacpy(&cp.bdaddr, BDADDR_ANY);
		cp.delete_all = 1;
		hci_send_cmd(hdev, HCI_OP_DELETE_STORED_LINK_KEY,
				sizeof(cp), &cp);
	} else {
		/* AMP initialization */
		/* Connection accept timeout ~5 secs */
		param = cpu_to_le16(0x1f40);
		hci_send_cmd(hdev, HCI_OP_WRITE_CA_TIMEOUT, 2, &param);

		/* Read AMP Info */
		hci_send_cmd(hdev, HCI_OP_READ_LOCAL_AMP_INFO, 0, NULL);
	}
=======
	if (!test_bit(HCI_QUIRK_RESET_ON_CLOSE, &hdev->quirks))
		hci_reset_req(req, 0);

	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		bredr_init(req);
		break;
	case HCI_AMP:
		amp_init1(req);
		break;
	default:
		BT_ERR("Unknown device type %d", hdev->dev_type);
		break;
	}

	return 0;
}

static void bredr_setup(struct hci_request *req)
{
	__le16 param;
	__u8 flt_type;

	/* Read Buffer Size (ACL mtu, max pkt, etc.) */
	hci_req_add(req, HCI_OP_READ_BUFFER_SIZE, 0, NULL);

	/* Read Class of Device */
	hci_req_add(req, HCI_OP_READ_CLASS_OF_DEV, 0, NULL);

	/* Read Local Name */
	hci_req_add(req, HCI_OP_READ_LOCAL_NAME, 0, NULL);

	/* Read Voice Setting */
	hci_req_add(req, HCI_OP_READ_VOICE_SETTING, 0, NULL);

	/* Read Number of Supported IAC */
	hci_req_add(req, HCI_OP_READ_NUM_SUPPORTED_IAC, 0, NULL);

	/* Read Current IAC LAP */
	hci_req_add(req, HCI_OP_READ_CURRENT_IAC_LAP, 0, NULL);

	/* Clear Event Filters */
	flt_type = HCI_FLT_CLEAR_ALL;
	hci_req_add(req, HCI_OP_SET_EVENT_FLT, 1, &flt_type);

	/* Connection accept timeout ~20 secs */
	param = cpu_to_le16(0x7d00);
	hci_req_add(req, HCI_OP_WRITE_CA_TIMEOUT, 2, &param);
}

static void le_setup(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;

	/* Read LE Buffer Size */
	hci_req_add(req, HCI_OP_LE_READ_BUFFER_SIZE, 0, NULL);

	/* Read LE Local Supported Features */
	hci_req_add(req, HCI_OP_LE_READ_LOCAL_FEATURES, 0, NULL);

	/* Read LE Supported States */
	hci_req_add(req, HCI_OP_LE_READ_SUPPORTED_STATES, 0, NULL);

	/* LE-only controllers have LE implicitly enabled */
	if (!lmp_bredr_capable(hdev))
		hci_dev_set_flag(hdev, HCI_LE_ENABLED);
}

static void hci_setup_event_mask(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;

	/* The second byte is 0xff instead of 0x9f (two reserved bits
	 * disabled) since a Broadcom 1.2 dongle doesn't respond to the
	 * command otherwise.
	 */
	u8 events[8] = { 0xff, 0xff, 0xfb, 0xff, 0x00, 0x00, 0x00, 0x00 };

	/* CSR 1.1 dongles does not accept any bitfield so don't try to set
	 * any event mask for pre 1.2 devices.
	 */
	if (hdev->hci_ver < BLUETOOTH_VER_1_2)
		return;

	if (lmp_bredr_capable(hdev)) {
		events[4] |= 0x01; /* Flow Specification Complete */
	} else {
		/* Use a different default for LE-only devices */
		memset(events, 0, sizeof(events));
		events[1] |= 0x20; /* Command Complete */
		events[1] |= 0x40; /* Command Status */
		events[1] |= 0x80; /* Hardware Error */

		/* If the controller supports the Disconnect command, enable
		 * the corresponding event. In addition enable packet flow
		 * control related events.
		 */
		if (hdev->commands[0] & 0x20) {
			events[0] |= 0x10; /* Disconnection Complete */
			events[2] |= 0x04; /* Number of Completed Packets */
			events[3] |= 0x02; /* Data Buffer Overflow */
		}

		/* If the controller supports the Read Remote Version
		 * Information command, enable the corresponding event.
		 */
		if (hdev->commands[2] & 0x80)
			events[1] |= 0x08; /* Read Remote Version Information
					    * Complete
					    */

		if (hdev->le_features[0] & HCI_LE_ENCRYPTION) {
			events[0] |= 0x80; /* Encryption Change */
			events[5] |= 0x80; /* Encryption Key Refresh Complete */
		}
	}

	if (lmp_inq_rssi_capable(hdev) ||
	    test_bit(HCI_QUIRK_FIXUP_INQUIRY_MODE, &hdev->quirks))
		events[4] |= 0x02; /* Inquiry Result with RSSI */

	if (lmp_ext_feat_capable(hdev))
		events[4] |= 0x04; /* Read Remote Extended Features Complete */

	if (lmp_esco_capable(hdev)) {
		events[5] |= 0x08; /* Synchronous Connection Complete */
		events[5] |= 0x10; /* Synchronous Connection Changed */
	}

	if (lmp_sniffsubr_capable(hdev))
		events[5] |= 0x20; /* Sniff Subrating */

	if (lmp_pause_enc_capable(hdev))
		events[5] |= 0x80; /* Encryption Key Refresh Complete */

	if (lmp_ext_inq_capable(hdev))
		events[5] |= 0x40; /* Extended Inquiry Result */

	if (lmp_no_flush_capable(hdev))
		events[7] |= 0x01; /* Enhanced Flush Complete */

	if (lmp_lsto_capable(hdev))
		events[6] |= 0x80; /* Link Supervision Timeout Changed */

	if (lmp_ssp_capable(hdev)) {
		events[6] |= 0x01;	/* IO Capability Request */
		events[6] |= 0x02;	/* IO Capability Response */
		events[6] |= 0x04;	/* User Confirmation Request */
		events[6] |= 0x08;	/* User Passkey Request */
		events[6] |= 0x10;	/* Remote OOB Data Request */
		events[6] |= 0x20;	/* Simple Pairing Complete */
		events[7] |= 0x04;	/* User Passkey Notification */
		events[7] |= 0x08;	/* Keypress Notification */
		events[7] |= 0x10;	/* Remote Host Supported
					 * Features Notification
					 */
	}

	if (lmp_le_capable(hdev))
		events[7] |= 0x20;	/* LE Meta-Event */

	hci_req_add(req, HCI_OP_SET_EVENT_MASK, sizeof(events), events);
}

static int hci_init2_req(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	if (hdev->dev_type == HCI_AMP)
		return amp_init2(req);

	if (lmp_bredr_capable(hdev))
		bredr_setup(req);
	else
		hci_dev_clear_flag(hdev, HCI_BREDR_ENABLED);

	if (lmp_le_capable(hdev))
		le_setup(req);

	/* All Bluetooth 1.2 and later controllers should support the
	 * HCI command for reading the local supported commands.
	 *
	 * Unfortunately some controllers indicate Bluetooth 1.2 support,
	 * but do not have support for this command. If that is the case,
	 * the driver can quirk the behavior and skip reading the local
	 * supported commands.
	 */
	if (hdev->hci_ver > BLUETOOTH_VER_1_1 &&
	    !test_bit(HCI_QUIRK_BROKEN_LOCAL_COMMANDS, &hdev->quirks))
		hci_req_add(req, HCI_OP_READ_LOCAL_COMMANDS, 0, NULL);

	if (lmp_ssp_capable(hdev)) {
		/* When SSP is available, then the host features page
		 * should also be available as well. However some
		 * controllers list the max_page as 0 as long as SSP
		 * has not been enabled. To achieve proper debugging
		 * output, force the minimum max_page to 1 at least.
		 */
		hdev->max_page = 0x01;

		if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED)) {
			u8 mode = 0x01;

			hci_req_add(req, HCI_OP_WRITE_SSP_MODE,
				    sizeof(mode), &mode);
		} else {
			struct hci_cp_write_eir cp;

			memset(hdev->eir, 0, sizeof(hdev->eir));
			memset(&cp, 0, sizeof(cp));

			hci_req_add(req, HCI_OP_WRITE_EIR, sizeof(cp), &cp);
		}
	}

	if (lmp_inq_rssi_capable(hdev) ||
	    test_bit(HCI_QUIRK_FIXUP_INQUIRY_MODE, &hdev->quirks)) {
		u8 mode;

		/* If Extended Inquiry Result events are supported, then
		 * they are clearly preferred over Inquiry Result with RSSI
		 * events.
		 */
		mode = lmp_ext_inq_capable(hdev) ? 0x02 : 0x01;

		hci_req_add(req, HCI_OP_WRITE_INQUIRY_MODE, 1, &mode);
	}

	if (lmp_inq_tx_pwr_capable(hdev))
		hci_req_add(req, HCI_OP_READ_INQ_RSP_TX_POWER, 0, NULL);

	if (lmp_ext_feat_capable(hdev)) {
		struct hci_cp_read_local_ext_features cp;

		cp.page = 0x01;
		hci_req_add(req, HCI_OP_READ_LOCAL_EXT_FEATURES,
			    sizeof(cp), &cp);
	}

	if (hci_dev_test_flag(hdev, HCI_LINK_SECURITY)) {
		u8 enable = 1;
		hci_req_add(req, HCI_OP_WRITE_AUTH_ENABLE, sizeof(enable),
			    &enable);
	}

	return 0;
>>>>>>> android-4.9
}

static void hci_setup_link_policy(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_write_def_link_policy cp;
	u16 link_policy = 0;

	if (lmp_rswitch_capable(hdev))
		link_policy |= HCI_LP_RSWITCH;
	if (lmp_hold_capable(hdev))
		link_policy |= HCI_LP_HOLD;
	if (lmp_sniff_capable(hdev))
		link_policy |= HCI_LP_SNIFF;
	if (lmp_park_capable(hdev))
		link_policy |= HCI_LP_PARK;

	cp.policy = cpu_to_le16(link_policy);
	hci_req_add(req, HCI_OP_WRITE_DEF_LINK_POLICY, sizeof(cp), &cp);
}

static void hci_set_le_support(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_write_le_host_supported cp;

	/* LE-only devices do not support explicit enablement */
	if (!lmp_bredr_capable(hdev))
		return;

	memset(&cp, 0, sizeof(cp));

	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		cp.le = 0x01;
		cp.simul = 0x00;
	}

	if (cp.le != lmp_host_le_capable(hdev))
		hci_req_add(req, HCI_OP_WRITE_LE_HOST_SUPPORTED, sizeof(cp),
			    &cp);
}

static void hci_set_event_mask_page_2(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	u8 events[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	/* If Connectionless Slave Broadcast master role is supported
	 * enable all necessary events for it.
	 */
	if (lmp_csb_master_capable(hdev)) {
		events[1] |= 0x40;	/* Triggered Clock Capture */
		events[1] |= 0x80;	/* Synchronization Train Complete */
		events[2] |= 0x10;	/* Slave Page Response Timeout */
		events[2] |= 0x20;	/* CSB Channel Map Change */
	}

	/* If Connectionless Slave Broadcast slave role is supported
	 * enable all necessary events for it.
	 */
	if (lmp_csb_slave_capable(hdev)) {
		events[2] |= 0x01;	/* Synchronization Train Received */
		events[2] |= 0x02;	/* CSB Receive */
		events[2] |= 0x04;	/* CSB Timeout */
		events[2] |= 0x08;	/* Truncated Page Complete */
	}

	/* Enable Authenticated Payload Timeout Expired event if supported */
	if (lmp_ping_capable(hdev) || hdev->le_features[0] & HCI_LE_PING)
		events[2] |= 0x80;

	hci_req_add(req, HCI_OP_SET_EVENT_MASK_PAGE_2, sizeof(events), events);
}

static int hci_init3_req(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;
	u8 p;

	hci_setup_event_mask(req);

	if (hdev->commands[6] & 0x20 &&
	    !test_bit(HCI_QUIRK_BROKEN_STORED_LINK_KEY, &hdev->quirks)) {
		struct hci_cp_read_stored_link_key cp;

		bacpy(&cp.bdaddr, BDADDR_ANY);
		cp.read_all = 0x01;
		hci_req_add(req, HCI_OP_READ_STORED_LINK_KEY, sizeof(cp), &cp);
	}

	if (hdev->commands[5] & 0x10)
		hci_setup_link_policy(req);

	if (hdev->commands[8] & 0x01)
		hci_req_add(req, HCI_OP_READ_PAGE_SCAN_ACTIVITY, 0, NULL);

	/* Some older Broadcom based Bluetooth 1.2 controllers do not
	 * support the Read Page Scan Type command. Check support for
	 * this command in the bit mask of supported commands.
	 */
	if (hdev->commands[13] & 0x01)
		hci_req_add(req, HCI_OP_READ_PAGE_SCAN_TYPE, 0, NULL);

	if (lmp_le_capable(hdev)) {
		u8 events[8];

		memset(events, 0, sizeof(events));

		if (hdev->le_features[0] & HCI_LE_ENCRYPTION)
			events[0] |= 0x10;	/* LE Long Term Key Request */

		/* If controller supports the Connection Parameters Request
		 * Link Layer Procedure, enable the corresponding event.
		 */
		if (hdev->le_features[0] & HCI_LE_CONN_PARAM_REQ_PROC)
			events[0] |= 0x20;	/* LE Remote Connection
						 * Parameter Request
						 */

		/* If the controller supports the Data Length Extension
		 * feature, enable the corresponding event.
		 */
		if (hdev->le_features[0] & HCI_LE_DATA_LEN_EXT)
			events[0] |= 0x40;	/* LE Data Length Change */

		/* If the controller supports Extended Scanner Filter
		 * Policies, enable the correspondig event.
		 */
		if (hdev->le_features[0] & HCI_LE_EXT_SCAN_POLICY)
			events[1] |= 0x04;	/* LE Direct Advertising
						 * Report
						 */

		/* If the controller supports the LE Set Scan Enable command,
		 * enable the corresponding advertising report event.
		 */
		if (hdev->commands[26] & 0x08)
			events[0] |= 0x02;	/* LE Advertising Report */

		/* If the controller supports the LE Create Connection
		 * command, enable the corresponding event.
		 */
		if (hdev->commands[26] & 0x10)
			events[0] |= 0x01;	/* LE Connection Complete */

		/* If the controller supports the LE Connection Update
		 * command, enable the corresponding event.
		 */
		if (hdev->commands[27] & 0x04)
			events[0] |= 0x04;	/* LE Connection Update
						 * Complete
						 */

		/* If the controller supports the LE Read Remote Used Features
		 * command, enable the corresponding event.
		 */
		if (hdev->commands[27] & 0x20)
			events[0] |= 0x08;	/* LE Read Remote Used
						 * Features Complete
						 */

		/* If the controller supports the LE Read Local P-256
		 * Public Key command, enable the corresponding event.
		 */
		if (hdev->commands[34] & 0x02)
			events[0] |= 0x80;	/* LE Read Local P-256
						 * Public Key Complete
						 */

		/* If the controller supports the LE Generate DHKey
		 * command, enable the corresponding event.
		 */
		if (hdev->commands[34] & 0x04)
			events[1] |= 0x01;	/* LE Generate DHKey Complete */

		hci_req_add(req, HCI_OP_LE_SET_EVENT_MASK, sizeof(events),
			    events);

		if (hdev->commands[25] & 0x40) {
			/* Read LE Advertising Channel TX Power */
			hci_req_add(req, HCI_OP_LE_READ_ADV_TX_POWER, 0, NULL);
		}

		if (hdev->commands[26] & 0x40) {
			/* Read LE White List Size */
			hci_req_add(req, HCI_OP_LE_READ_WHITE_LIST_SIZE,
				    0, NULL);
		}

		if (hdev->commands[26] & 0x80) {
			/* Clear LE White List */
			hci_req_add(req, HCI_OP_LE_CLEAR_WHITE_LIST, 0, NULL);
		}

		if (hdev->le_features[0] & HCI_LE_DATA_LEN_EXT) {
			/* Read LE Maximum Data Length */
			hci_req_add(req, HCI_OP_LE_READ_MAX_DATA_LEN, 0, NULL);

			/* Read LE Suggested Default Data Length */
			hci_req_add(req, HCI_OP_LE_READ_DEF_DATA_LEN, 0, NULL);
		}

		hci_set_le_support(req);
	}

	/* Read features beyond page 1 if available */
	for (p = 2; p < HCI_MAX_PAGES && p <= hdev->max_page; p++) {
		struct hci_cp_read_local_ext_features cp;

		cp.page = p;
		hci_req_add(req, HCI_OP_READ_LOCAL_EXT_FEATURES,
			    sizeof(cp), &cp);
	}

	return 0;
}

static int hci_init4_req(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	/* Some Broadcom based Bluetooth controllers do not support the
	 * Delete Stored Link Key command. They are clearly indicating its
	 * absence in the bit mask of supported commands.
	 *
	 * Check the supported commands and only if the the command is marked
	 * as supported send it. If not supported assume that the controller
	 * does not have actual support for stored link keys which makes this
	 * command redundant anyway.
	 *
	 * Some controllers indicate that they support handling deleting
	 * stored link keys, but they don't. The quirk lets a driver
	 * just disable this command.
	 */
	if (hdev->commands[6] & 0x80 &&
	    !test_bit(HCI_QUIRK_BROKEN_STORED_LINK_KEY, &hdev->quirks)) {
		struct hci_cp_delete_stored_link_key cp;

		bacpy(&cp.bdaddr, BDADDR_ANY);
		cp.delete_all = 0x01;
		hci_req_add(req, HCI_OP_DELETE_STORED_LINK_KEY,
			    sizeof(cp), &cp);
	}

	/* Set event mask page 2 if the HCI command for it is supported */
	if (hdev->commands[22] & 0x04)
		hci_set_event_mask_page_2(req);

	/* Read local codec list if the HCI command is supported */
	if (hdev->commands[29] & 0x20)
		hci_req_add(req, HCI_OP_READ_LOCAL_CODECS, 0, NULL);

	/* Get MWS transport configuration if the HCI command is supported */
	if (hdev->commands[30] & 0x08)
		hci_req_add(req, HCI_OP_GET_MWS_TRANSPORT_CONFIG, 0, NULL);

	/* Check for Synchronization Train support */
	if (lmp_sync_train_capable(hdev))
		hci_req_add(req, HCI_OP_READ_SYNC_TRAIN_PARAMS, 0, NULL);

	/* Enable Secure Connections if supported and configured */
	if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED) &&
	    bredr_sc_enabled(hdev)) {
		u8 support = 0x01;

		hci_req_add(req, HCI_OP_WRITE_SC_SUPPORT,
			    sizeof(support), &support);
	}

<<<<<<< HEAD
	/* Read LE buffer size */
	hci_send_cmd(hdev, HCI_OP_LE_READ_BUFFER_SIZE, 0, NULL);

	/* Read LE clear white list */
	hci_send_cmd(hdev, HCI_OP_LE_CLEAR_WHITE_LIST, 0, NULL);

	/* Read LE white list size */
	hci_send_cmd(hdev, HCI_OP_LE_READ_WHITE_LIST_SIZE, 0, NULL);
=======
	return 0;
>>>>>>> android-4.9
}

static int __hci_init(struct hci_dev *hdev)
{
	int err;

	err = __hci_req_sync(hdev, hci_init1_req, 0, HCI_INIT_TIMEOUT, NULL);
	if (err < 0)
		return err;

	if (hci_dev_test_flag(hdev, HCI_SETUP))
		hci_debugfs_create_basic(hdev);

	err = __hci_req_sync(hdev, hci_init2_req, 0, HCI_INIT_TIMEOUT, NULL);
	if (err < 0)
		return err;

	/* HCI_PRIMARY covers both single-mode LE, BR/EDR and dual-mode
	 * BR/EDR/LE type controllers. AMP controllers only need the
	 * first two stages of init.
	 */
	if (hdev->dev_type != HCI_PRIMARY)
		return 0;

	err = __hci_req_sync(hdev, hci_init3_req, 0, HCI_INIT_TIMEOUT, NULL);
	if (err < 0)
		return err;

	err = __hci_req_sync(hdev, hci_init4_req, 0, HCI_INIT_TIMEOUT, NULL);
	if (err < 0)
		return err;

	/* This function is only called when the controller is actually in
	 * configured state. When the controller is marked as unconfigured,
	 * this initialization procedure is not run.
	 *
	 * It means that it is possible that a controller runs through its
	 * setup phase and then discovers missing settings. If that is the
	 * case, then this function will not be called. It then will only
	 * be called during the config phase.
	 *
	 * So only when in setup phase or config phase, create the debugfs
	 * entries and register the SMP channels.
	 */
	if (!hci_dev_test_flag(hdev, HCI_SETUP) &&
	    !hci_dev_test_flag(hdev, HCI_CONFIG))
		return 0;

	hci_debugfs_create_common(hdev);

	if (lmp_bredr_capable(hdev))
		hci_debugfs_create_bredr(hdev);

	if (lmp_le_capable(hdev))
		hci_debugfs_create_le(hdev);

	return 0;
}

static int hci_init0_req(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	BT_DBG("%s %ld", hdev->name, opt);

	/* Reset */
	if (!test_bit(HCI_QUIRK_RESET_ON_CLOSE, &hdev->quirks))
		hci_reset_req(req, 0);

	/* Read Local Version */
	hci_req_add(req, HCI_OP_READ_LOCAL_VERSION, 0, NULL);

	/* Read BD Address */
	if (hdev->set_bdaddr)
		hci_req_add(req, HCI_OP_READ_BD_ADDR, 0, NULL);

	return 0;
}

static int __hci_unconf_init(struct hci_dev *hdev)
{
	int err;

	if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
		return 0;

	err = __hci_req_sync(hdev, hci_init0_req, 0, HCI_INIT_TIMEOUT, NULL);
	if (err < 0)
		return err;

	if (hci_dev_test_flag(hdev, HCI_SETUP))
		hci_debugfs_create_basic(hdev);

	return 0;
}

static int hci_scan_req(struct hci_request *req, unsigned long opt)
{
	__u8 scan = opt;

	BT_DBG("%s %x", req->hdev->name, scan);

	/* Inquiry and Page scans */
	hci_req_add(req, HCI_OP_WRITE_SCAN_ENABLE, 1, &scan);
	return 0;
}

static int hci_auth_req(struct hci_request *req, unsigned long opt)
{
	__u8 auth = opt;

	BT_DBG("%s %x", req->hdev->name, auth);

	/* Authentication */
	hci_req_add(req, HCI_OP_WRITE_AUTH_ENABLE, 1, &auth);
	return 0;
}

static int hci_encrypt_req(struct hci_request *req, unsigned long opt)
{
	__u8 encrypt = opt;

	BT_DBG("%s %x", req->hdev->name, encrypt);

	/* Encryption */
	hci_req_add(req, HCI_OP_WRITE_ENCRYPT_MODE, 1, &encrypt);
	return 0;
}

static int hci_linkpol_req(struct hci_request *req, unsigned long opt)
{
	__le16 policy = cpu_to_le16(opt);

	BT_DBG("%s %x", req->hdev->name, policy);

	/* Default link policy */
	hci_req_add(req, HCI_OP_WRITE_DEF_LINK_POLICY, 2, &policy);
	return 0;
}

/* Get HCI device by index.
 * Device is held on return. */
struct hci_dev *hci_dev_get(int index)
{
	struct hci_dev *hdev = NULL;
	struct list_head *p;

	BT_DBG("%d", index);

	if (index < 0)
		return NULL;

	read_lock(&hci_dev_list_lock);
	list_for_each(p, &hci_dev_list) {
		struct hci_dev *d = list_entry(p, struct hci_dev, list);
		if (d->id == index) {
			hdev = hci_dev_hold(d);
			break;
		}
	}
	read_unlock(&hci_dev_list_lock);
	return hdev;
}
EXPORT_SYMBOL(hci_dev_get);

/* ---- Inquiry support ---- */
<<<<<<< HEAD
static void inquiry_cache_flush(struct hci_dev *hdev)
{
	struct inquiry_cache *cache = &hdev->inq_cache;
	struct inquiry_entry *next  = cache->list, *e;

	BT_DBG("cache %p", cache);
=======

bool hci_discovery_active(struct hci_dev *hdev)
{
	struct discovery_state *discov = &hdev->discovery;

	switch (discov->state) {
	case DISCOVERY_FINDING:
	case DISCOVERY_RESOLVING:
		return true;

	default:
		return false;
	}
}

void hci_discovery_set_state(struct hci_dev *hdev, int state)
{
	int old_state = hdev->discovery.state;

	BT_DBG("%s state %u -> %u", hdev->name, hdev->discovery.state, state);

	if (old_state == state)
		return;

	hdev->discovery.state = state;

	switch (state) {
	case DISCOVERY_STOPPED:
		hci_update_background_scan(hdev);

		if (old_state != DISCOVERY_STARTING)
			mgmt_discovering(hdev, 0);
		break;
	case DISCOVERY_STARTING:
		break;
	case DISCOVERY_FINDING:
		mgmt_discovering(hdev, 1);
		break;
	case DISCOVERY_RESOLVING:
		break;
	case DISCOVERY_STOPPING:
		break;
	}
}

void hci_inquiry_cache_flush(struct hci_dev *hdev)
{
	struct discovery_state *cache = &hdev->discovery;
	struct inquiry_entry *p, *n;

	list_for_each_entry_safe(p, n, &cache->all, all) {
		list_del(&p->all);
		kfree(p);
	}

	INIT_LIST_HEAD(&cache->unknown);
	INIT_LIST_HEAD(&cache->resolve);
}

struct inquiry_entry *hci_inquiry_cache_lookup(struct hci_dev *hdev,
					       bdaddr_t *bdaddr)
{
	struct discovery_state *cache = &hdev->discovery;
	struct inquiry_entry *e;

	BT_DBG("cache %p, %pMR", cache, bdaddr);
>>>>>>> android-4.9

	cache->list = NULL;
	while ((e = next)) {
		next = e->next;
		kfree(e);
	}
}

struct inquiry_entry *hci_inquiry_cache_lookup(struct hci_dev *hdev, bdaddr_t *bdaddr)
{
	struct inquiry_cache *cache = &hdev->inq_cache;
	struct inquiry_entry *e;

	BT_DBG("cache %p, %pMR", cache, bdaddr);

<<<<<<< HEAD
	for (e = cache->list; e; e = e->next)
		if (!bacmp(&e->data.bdaddr, bdaddr))
=======
	list_for_each_entry(e, &cache->unknown, list) {
		if (!bacmp(&e->data.bdaddr, bdaddr))
			return e;
	}

	return NULL;
}

struct inquiry_entry *hci_inquiry_cache_lookup_resolve(struct hci_dev *hdev,
						       bdaddr_t *bdaddr,
						       int state)
{
	struct discovery_state *cache = &hdev->discovery;
	struct inquiry_entry *e;

	BT_DBG("cache %p bdaddr %pMR state %d", cache, bdaddr, state);

	list_for_each_entry(e, &cache->resolve, list) {
		if (!bacmp(bdaddr, BDADDR_ANY) && e->name_state == state)
			return e;
		if (!bacmp(&e->data.bdaddr, bdaddr))
			return e;
	}

	return NULL;
}

void hci_inquiry_cache_update_resolve(struct hci_dev *hdev,
				      struct inquiry_entry *ie)
{
	struct discovery_state *cache = &hdev->discovery;
	struct list_head *pos = &cache->resolve;
	struct inquiry_entry *p;

	list_del(&ie->list);

	list_for_each_entry(p, &cache->resolve, list) {
		if (p->name_state != NAME_PENDING &&
		    abs(p->data.rssi) >= abs(ie->data.rssi))
>>>>>>> android-4.9
			break;
	return e;
}

<<<<<<< HEAD
void hci_inquiry_cache_update(struct hci_dev *hdev, struct inquiry_data *data)
=======
u32 hci_inquiry_cache_update(struct hci_dev *hdev, struct inquiry_data *data,
			     bool name_known)
>>>>>>> android-4.9
{
	struct inquiry_cache *cache = &hdev->inq_cache;
	struct inquiry_entry *ie;
	u32 flags = 0;

	BT_DBG("cache %p, %pMR", cache, &data->bdaddr);

<<<<<<< HEAD
	ie = hci_inquiry_cache_lookup(hdev, &data->bdaddr);
	if (!ie) {
		/* Entry not in the cache. Add new one. */
		ie = kzalloc(sizeof(struct inquiry_entry), GFP_ATOMIC);
		if (!ie)
			return;

		ie->next = cache->list;
		cache->list = ie;
=======
	hci_remove_remote_oob_data(hdev, &data->bdaddr, BDADDR_BREDR);

	if (!data->ssp_mode)
		flags |= MGMT_DEV_FOUND_LEGACY_PAIRING;

	ie = hci_inquiry_cache_lookup(hdev, &data->bdaddr);
	if (ie) {
		if (!ie->data.ssp_mode)
			flags |= MGMT_DEV_FOUND_LEGACY_PAIRING;

		if (ie->name_state == NAME_NEEDED &&
		    data->rssi != ie->data.rssi) {
			ie->data.rssi = data->rssi;
			hci_inquiry_cache_update_resolve(hdev, ie);
		}

		goto update;
	}

	/* Entry not in the cache. Add new one. */
	ie = kzalloc(sizeof(*ie), GFP_KERNEL);
	if (!ie) {
		flags |= MGMT_DEV_FOUND_CONFIRM_NAME;
		goto done;
	}

	list_add(&ie->all, &cache->all);

	if (name_known) {
		ie->name_state = NAME_KNOWN;
	} else {
		ie->name_state = NAME_NOT_KNOWN;
		list_add(&ie->list, &cache->unknown);
	}

update:
	if (name_known && ie->name_state != NAME_KNOWN &&
	    ie->name_state != NAME_PENDING) {
		ie->name_state = NAME_KNOWN;
		list_del(&ie->list);
>>>>>>> android-4.9
	}

	memcpy(&ie->data, data, sizeof(*data));
	ie->timestamp = jiffies;
	cache->timestamp = jiffies;
<<<<<<< HEAD
=======

	if (ie->name_state == NAME_NOT_KNOWN)
		flags |= MGMT_DEV_FOUND_CONFIRM_NAME;

done:
	return flags;
>>>>>>> android-4.9
}

static int inquiry_cache_dump(struct hci_dev *hdev, int num, __u8 *buf)
{
	struct inquiry_cache *cache = &hdev->inq_cache;
	struct inquiry_info *info = (struct inquiry_info *) buf;
	struct inquiry_entry *e;
	int copied = 0;

	for (e = cache->list; e && copied < num; e = e->next, copied++) {
		struct inquiry_data *data = &e->data;
		bacpy(&info->bdaddr, &data->bdaddr);
		info->pscan_rep_mode	= data->pscan_rep_mode;
		info->pscan_period_mode	= data->pscan_period_mode;
		info->pscan_mode	= data->pscan_mode;
		memcpy(info->dev_class, data->dev_class, 3);
		info->clock_offset	= data->clock_offset;
		info++;
	}

	BT_DBG("cache %p, copied %d", cache, copied);
	return copied;
}

static int hci_inq_req(struct hci_request *req, unsigned long opt)
{
	struct hci_inquiry_req *ir = (struct hci_inquiry_req *) opt;
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_inquiry cp;

	BT_DBG("%s", hdev->name);

	if (test_bit(HCI_INQUIRY, &hdev->flags))
		return 0;

	/* Start Inquiry */
	memcpy(&cp.lap, &ir->lap, 3);
	cp.length  = ir->length;
	cp.num_rsp = ir->num_rsp;
	hci_req_add(req, HCI_OP_INQUIRY, sizeof(cp), &cp);

	return 0;
}

int hci_inquiry(void __user *arg)
{
	__u8 __user *ptr = arg;
	struct hci_inquiry_req ir;
	struct hci_dev *hdev;
	int err = 0, do_inquiry = 0, max_rsp;
	long timeo;
	__u8 *buf;

	if (copy_from_user(&ir, ptr, sizeof(ir)))
		return -EFAULT;

	hdev = hci_dev_get(ir.dev_id);
	if (!hdev)
		return -ENODEV;

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);
=======
	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		err = -EBUSY;
		goto done;
	}

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	if (hdev->dev_type != HCI_PRIMARY) {
		err = -EOPNOTSUPP;
		goto done;
	}

	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	hci_dev_lock(hdev);
>>>>>>> android-4.9
	if (inquiry_cache_age(hdev) > INQUIRY_CACHE_AGE_MAX ||
	    inquiry_cache_empty(hdev) || ir.flags & IREQ_CACHE_FLUSH) {
		hci_inquiry_cache_flush(hdev);
		do_inquiry = 1;
	}
	hci_dev_unlock_bh(hdev);

	timeo = ir.length * msecs_to_jiffies(2000);

	if (do_inquiry) {
		err = hci_req_sync(hdev, hci_inq_req, (unsigned long) &ir,
				   timeo, NULL);
		if (err < 0)
			goto done;

		/* Wait until Inquiry procedure finishes (HCI_INQUIRY flag is
		 * cleared). If it is interrupted by a signal, return -EINTR.
		 */
		if (wait_on_bit(&hdev->flags, HCI_INQUIRY,
				TASK_INTERRUPTIBLE))
			return -EINTR;
	}

	/* for unlimited number of responses we will use buffer with
	 * 255 entries
	 */
	max_rsp = (ir.num_rsp == 0) ? 255 : ir.num_rsp;

	/* cache_dump can't sleep. Therefore we allocate temp buffer and then
	 * copy it to the user space.
	 */
	buf = kmalloc(sizeof(struct inquiry_info) * max_rsp, GFP_KERNEL);
	if (!buf) {
		err = -ENOMEM;
		goto done;
	}

	hci_dev_lock_bh(hdev);
	ir.num_rsp = inquiry_cache_dump(hdev, max_rsp, buf);
	hci_dev_unlock_bh(hdev);

	BT_DBG("num_rsp %d", ir.num_rsp);

	if (!copy_to_user(ptr, &ir, sizeof(ir))) {
		ptr += sizeof(ir);
		if (copy_to_user(ptr, buf, sizeof(struct inquiry_info) *
				 ir.num_rsp))
			err = -EFAULT;
	} else
		err = -EFAULT;

	kfree(buf);

done:
	hci_dev_put(hdev);
	return err;
}

static int hci_dev_do_open(struct hci_dev *hdev)
{
	int ret = 0;

	BT_DBG("%s %p", hdev->name, hdev);

	hci_req_sync_lock(hdev);

<<<<<<< HEAD
	if (hdev->rfkill && rfkill_blocked(hdev->rfkill)) {
		ret = -ERFKILL;
		goto done;
=======
	if (hci_dev_test_flag(hdev, HCI_UNREGISTER)) {
		ret = -ENODEV;
		goto done;
	}

	if (!hci_dev_test_flag(hdev, HCI_SETUP) &&
	    !hci_dev_test_flag(hdev, HCI_CONFIG)) {
		/* Check for rfkill but allow the HCI setup stage to
		 * proceed (which in itself doesn't cause any RF activity).
		 */
		if (hci_dev_test_flag(hdev, HCI_RFKILLED)) {
			ret = -ERFKILL;
			goto done;
		}

		/* Check for valid public address or a configured static
		 * random adddress, but let the HCI setup proceed to
		 * be able to determine if there is a public address
		 * or not.
		 *
		 * In case of user channel usage, it is not important
		 * if a public address or static random address is
		 * available.
		 *
		 * This check is only valid for BR/EDR controllers
		 * since AMP controllers do not have an address.
		 */
		if (!hci_dev_test_flag(hdev, HCI_USER_CHANNEL) &&
		    hdev->dev_type == HCI_PRIMARY &&
		    !bacmp(&hdev->bdaddr, BDADDR_ANY) &&
		    !bacmp(&hdev->static_addr, BDADDR_ANY)) {
			ret = -EADDRNOTAVAIL;
			goto done;
		}
>>>>>>> android-4.9
	}

	if (test_bit(HCI_UP, &hdev->flags)) {
		ret = -EALREADY;
		goto done;
	}

<<<<<<< HEAD
	if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
		set_bit(HCI_RAW, &hdev->flags);

=======
>>>>>>> android-4.9
	if (hdev->open(hdev)) {
		ret = -EIO;
		goto done;
	}

<<<<<<< HEAD
	if (!skb_queue_empty(&hdev->cmd_q)) {
		BT_ERR("command queue is not empty, purging");
		skb_queue_purge(&hdev->cmd_q);
	}
	if (!skb_queue_empty(&hdev->rx_q)) {
		BT_ERR("rx queue is not empty, purging");
		skb_queue_purge(&hdev->rx_q);
	}
	if (!skb_queue_empty(&hdev->raw_q)) {
		BT_ERR("raw queue is not empty, purging");
		skb_queue_purge(&hdev->raw_q);
	}

	if (!test_bit(HCI_RAW, &hdev->flags)) {
		atomic_set(&hdev->cmd_cnt, 1);
		set_bit(HCI_INIT, &hdev->flags);
		hdev->init_last_cmd = 0;
=======
	set_bit(HCI_RUNNING, &hdev->flags);
	hci_sock_dev_event(hdev, HCI_DEV_OPEN);

	atomic_set(&hdev->cmd_cnt, 1);
	set_bit(HCI_INIT, &hdev->flags);
>>>>>>> android-4.9

	if (hci_dev_test_flag(hdev, HCI_SETUP)) {
		hci_sock_dev_event(hdev, HCI_DEV_SETUP);

<<<<<<< HEAD
		if (lmp_le_capable(hdev))
			ret = __hci_request(hdev, hci_le_init_req, 0,
					msecs_to_jiffies(HCI_INIT_TIMEOUT));
=======
		if (hdev->setup)
			ret = hdev->setup(hdev);
>>>>>>> android-4.9

		/* The transport driver can set these quirks before
		 * creating the HCI device or in its setup callback.
		 *
		 * In case any of them is set, the controller has to
		 * start up as unconfigured.
		 */
		if (test_bit(HCI_QUIRK_EXTERNAL_CONFIG, &hdev->quirks) ||
		    test_bit(HCI_QUIRK_INVALID_BDADDR, &hdev->quirks))
			hci_dev_set_flag(hdev, HCI_UNCONFIGURED);

		/* For an unconfigured controller it is required to
		 * read at least the version information provided by
		 * the Read Local Version Information command.
		 *
		 * If the set_bdaddr driver callback is provided, then
		 * also the original Bluetooth public device address
		 * will be read using the Read BD Address command.
		 */
		if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED))
			ret = __hci_unconf_init(hdev);
	}

	if (hci_dev_test_flag(hdev, HCI_CONFIG)) {
		/* If public address change is configured, ensure that
		 * the address gets programmed. If the driver does not
		 * support changing the public address, fail the power
		 * on procedure.
		 */
		if (bacmp(&hdev->public_addr, BDADDR_ANY) &&
		    hdev->set_bdaddr)
			ret = hdev->set_bdaddr(hdev, &hdev->public_addr);
		else
			ret = -EADDRNOTAVAIL;
	}

	if (!ret) {
		if (!hci_dev_test_flag(hdev, HCI_UNCONFIGURED) &&
		    !hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
			ret = __hci_init(hdev);
			if (!ret && hdev->post_init)
				ret = hdev->post_init(hdev);
		}
	}

	/* If the HCI Reset command is clearing all diagnostic settings,
	 * then they need to be reprogrammed after the init procedure
	 * completed.
	 */
	if (test_bit(HCI_QUIRK_NON_PERSISTENT_DIAG, &hdev->quirks) &&
	    hci_dev_test_flag(hdev, HCI_VENDOR_DIAG) && hdev->set_diag)
		ret = hdev->set_diag(hdev, true);

	clear_bit(HCI_INIT, &hdev->flags);

	if (!ret) {
		hci_dev_hold(hdev);
		hci_dev_set_flag(hdev, HCI_RPA_EXPIRED);
		set_bit(HCI_UP, &hdev->flags);
<<<<<<< HEAD
		hci_notify(hdev, HCI_DEV_UP);
		if (!test_bit(HCI_SETUP, &hdev->flags) &&
				hdev->dev_type == HCI_BREDR) {
			hci_dev_lock_bh(hdev);
			mgmt_powered(hdev->id, 1);
			hci_dev_unlock_bh(hdev);
=======
		hci_sock_dev_event(hdev, HCI_DEV_UP);
		hci_leds_update_powered(hdev, true);
		if (!hci_dev_test_flag(hdev, HCI_SETUP) &&
		    !hci_dev_test_flag(hdev, HCI_CONFIG) &&
		    !hci_dev_test_flag(hdev, HCI_UNCONFIGURED) &&
		    !hci_dev_test_flag(hdev, HCI_USER_CHANNEL) &&
		    hci_dev_test_flag(hdev, HCI_MGMT) &&
		    hdev->dev_type == HCI_PRIMARY) {
			ret = __hci_req_hci_power_on(hdev);
			mgmt_power_on(hdev, ret);
>>>>>>> android-4.9
		}
	} else {
		/* Init failed, cleanup */
		tasklet_kill(&hdev->rx_task);
		tasklet_kill(&hdev->tx_task);
		tasklet_kill(&hdev->cmd_task);

		skb_queue_purge(&hdev->cmd_q);
		skb_queue_purge(&hdev->rx_q);

		if (hdev->flush)
			hdev->flush(hdev);

		if (hdev->sent_cmd) {
			kfree_skb(hdev->sent_cmd);
			hdev->sent_cmd = NULL;
		}

		clear_bit(HCI_RUNNING, &hdev->flags);
		hci_sock_dev_event(hdev, HCI_DEV_CLOSE);

		hdev->close(hdev);
		hdev->flags &= BIT(HCI_RAW);
	}

done:
	hci_req_sync_unlock(hdev);
	return ret;
}

<<<<<<< HEAD
static int hci_dev_do_close(struct hci_dev *hdev, u8 is_process)
{
	unsigned long keepflags = 0;

	BT_DBG("%s %p", hdev->name, hdev);
=======
/* ---- HCI ioctl helpers ---- */

int hci_dev_open(__u16 dev)
{
	struct hci_dev *hdev;
	int err;

	hdev = hci_dev_get(dev);
	if (!hdev)
		return -ENODEV;

	/* Devices that are marked as unconfigured can only be powered
	 * up as user channel. Trying to bring them up as normal devices
	 * will result into a failure. Only user channel operation is
	 * possible.
	 *
	 * When this function is called for a user channel, the flag
	 * HCI_USER_CHANNEL will be set first before attempting to
	 * open the device.
	 */
	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED) &&
	    !hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	/* We need to ensure that no other power on/off work is pending
	 * before proceeding to call hci_dev_do_open. This is
	 * particularly important if the setup procedure has not yet
	 * completed.
	 */
	if (hci_dev_test_and_clear_flag(hdev, HCI_AUTO_OFF))
		cancel_delayed_work(&hdev->power_off);

	/* After this call it is guaranteed that the setup procedure
	 * has finished. This means that error conditions like RFKILL
	 * or no valid public or static random address apply.
	 */
	flush_workqueue(hdev->req_workqueue);

	/* For controllers not using the management interface and that
	 * are brought up using legacy ioctl, set the HCI_BONDABLE bit
	 * so that pairing works for them. Once the management interface
	 * is in use this bit will be cleared again and userspace has
	 * to explicitly enable it.
	 */
	if (!hci_dev_test_flag(hdev, HCI_USER_CHANNEL) &&
	    !hci_dev_test_flag(hdev, HCI_MGMT))
		hci_dev_set_flag(hdev, HCI_BONDABLE);

	err = hci_dev_do_open(hdev);

done:
	hci_dev_put(hdev);
	return err;
}

/* This function requires the caller holds hdev->lock */
static void hci_pend_le_actions_clear(struct hci_dev *hdev)
{
	struct hci_conn_params *p;

	list_for_each_entry(p, &hdev->le_conn_params, list) {
		if (p->conn) {
			hci_conn_drop(p->conn);
			hci_conn_put(p->conn);
			p->conn = NULL;
		}
		list_del_init(&p->action);
	}

	BT_DBG("All LE pending actions cleared");
}

int hci_dev_do_close(struct hci_dev *hdev)
{
	bool auto_off;

	BT_DBG("%s %p", hdev->name, hdev);

	if (!hci_dev_test_flag(hdev, HCI_UNREGISTER) &&
	    !hci_dev_test_flag(hdev, HCI_USER_CHANNEL) &&
	    test_bit(HCI_UP, &hdev->flags)) {
		/* Execute vendor specific shutdown routine */
		if (hdev->shutdown)
			hdev->shutdown(hdev);
	}

	cancel_delayed_work(&hdev->power_off);
>>>>>>> android-4.9

	hci_request_cancel_all(hdev);
	hci_req_sync_lock(hdev);

	if (!test_and_clear_bit(HCI_UP, &hdev->flags)) {
		cancel_delayed_work_sync(&hdev->cmd_timer);
		hci_req_sync_unlock(hdev);
		return 0;
	}

<<<<<<< HEAD
	/* Kill RX and TX tasks */
	tasklet_kill(&hdev->rx_task);
	tasklet_kill(&hdev->tx_task);

	hci_dev_lock_bh(hdev);
	inquiry_cache_flush(hdev);
	hci_conn_hash_flush(hdev, is_process);
	hci_dev_unlock_bh(hdev);
=======
	hci_leds_update_powered(hdev, false);

	/* Flush RX and TX works */
	flush_work(&hdev->tx_work);
	flush_work(&hdev->rx_work);

	if (hdev->discov_timeout > 0) {
		hdev->discov_timeout = 0;
		hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
		hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);
	}

	if (hci_dev_test_and_clear_flag(hdev, HCI_SERVICE_CACHE))
		cancel_delayed_work(&hdev->service_cache);

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		cancel_delayed_work_sync(&hdev->rpa_expired);

	/* Avoid potential lockdep warnings from the *_flush() calls by
	 * ensuring the workqueue is empty up front.
	 */
	drain_workqueue(hdev->workqueue);

	hci_dev_lock(hdev);

	hci_discovery_set_state(hdev, DISCOVERY_STOPPED);

	auto_off = hci_dev_test_and_clear_flag(hdev, HCI_AUTO_OFF);

	if (!auto_off && hdev->dev_type == HCI_PRIMARY &&
	    !hci_dev_test_flag(hdev, HCI_USER_CHANNEL) &&
	    hci_dev_test_flag(hdev, HCI_MGMT))
		__mgmt_power_off(hdev);

	hci_inquiry_cache_flush(hdev);
	hci_pend_le_actions_clear(hdev);
	hci_conn_hash_flush(hdev);
	hci_dev_unlock(hdev);
>>>>>>> android-4.9

	smp_unregister(hdev);

	hci_sock_dev_event(hdev, HCI_DEV_DOWN);

	if (hdev->dev_type == HCI_BREDR) {
		hci_dev_lock_bh(hdev);
		mgmt_powered(hdev->id, 0);
		hci_dev_unlock_bh(hdev);
	}

	if (hdev->flush)
		hdev->flush(hdev);

	/* Reset device */
	skb_queue_purge(&hdev->cmd_q);
	atomic_set(&hdev->cmd_cnt, 1);
<<<<<<< HEAD
	if (!test_bit(HCI_RAW, &hdev->flags)) {
=======
	if (test_bit(HCI_QUIRK_RESET_ON_CLOSE, &hdev->quirks) &&
	    !auto_off && !hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
>>>>>>> android-4.9
		set_bit(HCI_INIT, &hdev->flags);
		__hci_req_sync(hdev, hci_reset_req, 0, HCI_CMD_TIMEOUT, NULL);
		clear_bit(HCI_INIT, &hdev->flags);
	}

	/* Kill cmd task */
	tasklet_kill(&hdev->cmd_task);

	/* Drop queues */
	skb_queue_purge(&hdev->rx_q);
	skb_queue_purge(&hdev->cmd_q);
	skb_queue_purge(&hdev->raw_q);

	/* Drop last sent command */
	if (hdev->sent_cmd) {
		cancel_delayed_work_sync(&hdev->cmd_timer);
		kfree_skb(hdev->sent_cmd);
		hdev->sent_cmd = NULL;
	}

	clear_bit(HCI_RUNNING, &hdev->flags);
	hci_sock_dev_event(hdev, HCI_DEV_CLOSE);

	/* After this point our queues are empty
	 * and no tasks are scheduled. */
	hdev->close(hdev);

<<<<<<< HEAD
	/* Clear only non-persistent flags */
	if (test_bit(HCI_MGMT, &hdev->flags))
		set_bit(HCI_MGMT, &keepflags);
	if (test_bit(HCI_LINK_KEYS, &hdev->flags))
		set_bit(HCI_LINK_KEYS, &keepflags);
	if (test_bit(HCI_DEBUG_KEYS, &hdev->flags))
		set_bit(HCI_DEBUG_KEYS, &keepflags);

	hdev->flags = keepflags;
=======
	/* Clear flags */
	hdev->flags &= BIT(HCI_RAW);
	hci_dev_clear_volatile_flags(hdev);

	/* Controller radio is available but is currently powered down */
	hdev->amp_status = AMP_STATUS_POWERED_DOWN;

	memset(hdev->eir, 0, sizeof(hdev->eir));
	memset(hdev->dev_class, 0, sizeof(hdev->dev_class));
	bacpy(&hdev->random_addr, BDADDR_ANY);
>>>>>>> android-4.9

	hci_req_sync_unlock(hdev);

	hci_dev_put(hdev);
	return 0;
}

int hci_dev_close(__u16 dev)
{
	struct hci_dev *hdev;
	int err;

	hdev = hci_dev_get(dev);
	if (!hdev)
		return -ENODEV;
<<<<<<< HEAD
	err = hci_dev_do_close(hdev, 1);
=======

	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		err = -EBUSY;
		goto done;
	}

	if (hci_dev_test_and_clear_flag(hdev, HCI_AUTO_OFF))
		cancel_delayed_work(&hdev->power_off);

	err = hci_dev_do_close(hdev);

done:
>>>>>>> android-4.9
	hci_dev_put(hdev);
	return err;
}

static int hci_dev_do_reset(struct hci_dev *hdev)
{
	int ret;

<<<<<<< HEAD
	hci_req_lock(hdev);
	tasklet_disable(&hdev->tx_task);
=======
	BT_DBG("%s %p", hdev->name, hdev);
>>>>>>> android-4.9

	hci_req_sync_lock(hdev);

	/* Drop queues */
	skb_queue_purge(&hdev->rx_q);
	skb_queue_purge(&hdev->cmd_q);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);
	inquiry_cache_flush(hdev);
	hci_conn_hash_flush(hdev, 0);
	hci_dev_unlock_bh(hdev);
=======
	/* Avoid potential lockdep warnings from the *_flush() calls by
	 * ensuring the workqueue is empty up front.
	 */
	drain_workqueue(hdev->workqueue);

	hci_dev_lock(hdev);
	hci_inquiry_cache_flush(hdev);
	hci_conn_hash_flush(hdev);
	hci_dev_unlock(hdev);
>>>>>>> android-4.9

	if (hdev->flush)
		hdev->flush(hdev);

	atomic_set(&hdev->cmd_cnt, 1);
	hdev->acl_cnt = 0; hdev->sco_cnt = 0; hdev->le_cnt = 0;

	ret = __hci_req_sync(hdev, hci_reset_req, 0, HCI_INIT_TIMEOUT, NULL);

	hci_req_sync_unlock(hdev);
	return ret;
}

int hci_dev_reset(__u16 dev)
{
	struct hci_dev *hdev;
	int err;

	hdev = hci_dev_get(dev);
	if (!hdev)
		return -ENODEV;

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = -ENETDOWN;
		goto done;
	}

	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		err = -EBUSY;
		goto done;
	}

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	err = hci_dev_do_reset(hdev);

done:
<<<<<<< HEAD
	tasklet_enable(&hdev->tx_task);
	hci_req_unlock(hdev);
=======
>>>>>>> android-4.9
	hci_dev_put(hdev);
	return err;
}

int hci_dev_reset_stat(__u16 dev)
{
	struct hci_dev *hdev;
	int ret = 0;

	hdev = hci_dev_get(dev);
	if (!hdev)
		return -ENODEV;

	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		ret = -EBUSY;
		goto done;
	}

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
		ret = -EOPNOTSUPP;
		goto done;
	}

	memset(&hdev->stat, 0, sizeof(struct hci_dev_stats));

done:
	hci_dev_put(hdev);
	return ret;
}

static void hci_update_scan_state(struct hci_dev *hdev, u8 scan)
{
	bool conn_changed, discov_changed;

	BT_DBG("%s scan 0x%02x", hdev->name, scan);

	if ((scan & SCAN_PAGE))
		conn_changed = !hci_dev_test_and_set_flag(hdev,
							  HCI_CONNECTABLE);
	else
		conn_changed = hci_dev_test_and_clear_flag(hdev,
							   HCI_CONNECTABLE);

	if ((scan & SCAN_INQUIRY)) {
		discov_changed = !hci_dev_test_and_set_flag(hdev,
							    HCI_DISCOVERABLE);
	} else {
		hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);
		discov_changed = hci_dev_test_and_clear_flag(hdev,
							     HCI_DISCOVERABLE);
	}

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		return;

	if (conn_changed || discov_changed) {
		/* In case this was disabled through mgmt */
		hci_dev_set_flag(hdev, HCI_BREDR_ENABLED);

		if (hci_dev_test_flag(hdev, HCI_LE_ENABLED))
			hci_req_update_adv_data(hdev, hdev->cur_adv_instance);

		mgmt_new_settings(hdev);
	}
}

int hci_dev_cmd(unsigned int cmd, void __user *arg)
{
	struct hci_dev *hdev;
	struct hci_dev_req dr;
	int err = 0;

	if (copy_from_user(&dr, arg, sizeof(dr)))
		return -EFAULT;

	hdev = hci_dev_get(dr.dev_id);
	if (!hdev)
		return -ENODEV;

	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		err = -EBUSY;
		goto done;
	}

	if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	if (hdev->dev_type != HCI_PRIMARY) {
		err = -EOPNOTSUPP;
		goto done;
	}

	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		err = -EOPNOTSUPP;
		goto done;
	}

	switch (cmd) {
	case HCISETAUTH:
		err = hci_req_sync(hdev, hci_auth_req, dr.dev_opt,
				   HCI_INIT_TIMEOUT, NULL);
		break;

	case HCISETENCRYPT:
		if (!lmp_encrypt_capable(hdev)) {
			err = -EOPNOTSUPP;
			break;
		}

		if (!test_bit(HCI_AUTH, &hdev->flags)) {
			/* Auth must be enabled first */
			err = hci_req_sync(hdev, hci_auth_req, dr.dev_opt,
					   HCI_INIT_TIMEOUT, NULL);
			if (err)
				break;
		}

		err = hci_req_sync(hdev, hci_encrypt_req, dr.dev_opt,
				   HCI_INIT_TIMEOUT, NULL);
		break;

	case HCISETSCAN:
		err = hci_req_sync(hdev, hci_scan_req, dr.dev_opt,
				   HCI_INIT_TIMEOUT, NULL);

		/* Ensure that the connectable and discoverable states
		 * get correctly modified as this was a non-mgmt change.
		 */
		if (!err)
			hci_update_scan_state(hdev, dr.dev_opt);
		break;

	case HCISETLINKPOL:
		err = hci_req_sync(hdev, hci_linkpol_req, dr.dev_opt,
				   HCI_INIT_TIMEOUT, NULL);
		break;

	case HCISETLINKMODE:
		hdev->link_mode = ((__u16) dr.dev_opt) &
					(HCI_LM_MASTER | HCI_LM_ACCEPT);
		break;

	case HCISETPTYPE:
		hdev->pkt_type = (__u16) dr.dev_opt;
		break;

	case HCISETACLMTU:
		hdev->acl_mtu  = *((__u16 *) &dr.dev_opt + 1);
		hdev->acl_pkts = *((__u16 *) &dr.dev_opt + 0);
		break;

	case HCISETSCOMTU:
		hdev->sco_mtu  = *((__u16 *) &dr.dev_opt + 1);
		hdev->sco_pkts = *((__u16 *) &dr.dev_opt + 0);
		break;

	default:
		err = -EINVAL;
		break;
	}

done:
	hci_dev_put(hdev);
	return err;
}

int hci_get_dev_list(void __user *arg)
{
	struct hci_dev_list_req *dl;
	struct hci_dev_req *dr;
	struct list_head *p;
	int n = 0, size, err;
	__u16 dev_num;

	if (get_user(dev_num, (__u16 __user *) arg))
		return -EFAULT;

	if (!dev_num || dev_num > (PAGE_SIZE * 2) / sizeof(*dr))
		return -EINVAL;

	size = sizeof(*dl) + dev_num * sizeof(*dr);

	dl = kzalloc(size, GFP_KERNEL);
	if (!dl)
		return -ENOMEM;

	dr = dl->dev_req;

<<<<<<< HEAD
	read_lock_bh(&hci_dev_list_lock);
	list_for_each(p, &hci_dev_list) {
		struct hci_dev *hdev;

		hdev = list_entry(p, struct hci_dev, list);

		hci_del_off_timer(hdev);

		if (!test_bit(HCI_MGMT, &hdev->flags))
			set_bit(HCI_PAIRABLE, &hdev->flags);
=======
	read_lock(&hci_dev_list_lock);
	list_for_each_entry(hdev, &hci_dev_list, list) {
		unsigned long flags = hdev->flags;

		/* When the auto-off is configured it means the transport
		 * is running, but in that case still indicate that the
		 * device is actually down.
		 */
		if (hci_dev_test_flag(hdev, HCI_AUTO_OFF))
			flags &= ~BIT(HCI_UP);
>>>>>>> android-4.9

		(dr + n)->dev_id  = hdev->id;
		(dr + n)->dev_opt = flags;

		if (++n >= dev_num)
			break;
	}
	read_unlock_bh(&hci_dev_list_lock);

	dl->dev_num = n;
	size = sizeof(*dl) + n * sizeof(*dr);

	err = copy_to_user(arg, dl, size);
	kfree(dl);

	return err ? -EFAULT : 0;
}

int hci_get_dev_info(void __user *arg)
{
	struct hci_dev *hdev;
	struct hci_dev_info di;
	unsigned long flags;
	int err = 0;

	if (copy_from_user(&di, arg, sizeof(di)))
		return -EFAULT;

	hdev = hci_dev_get(di.dev_id);
	if (!hdev)
		return -ENODEV;

<<<<<<< HEAD
	hci_del_off_timer(hdev);

	if (!test_bit(HCI_MGMT, &hdev->flags))
		set_bit(HCI_PAIRABLE, &hdev->flags);
=======
	/* When the auto-off is configured it means the transport
	 * is running, but in that case still indicate that the
	 * device is actually down.
	 */
	if (hci_dev_test_flag(hdev, HCI_AUTO_OFF))
		flags = hdev->flags & ~BIT(HCI_UP);
	else
		flags = hdev->flags;
>>>>>>> android-4.9

	strcpy(di.name, hdev->name);
	di.bdaddr   = hdev->bdaddr;
	di.type     = (hdev->bus & 0x0f) | ((hdev->dev_type & 0x03) << 4);
	di.flags    = flags;
	di.pkt_type = hdev->pkt_type;
	if (lmp_bredr_capable(hdev)) {
		di.acl_mtu  = hdev->acl_mtu;
		di.acl_pkts = hdev->acl_pkts;
		di.sco_mtu  = hdev->sco_mtu;
		di.sco_pkts = hdev->sco_pkts;
	} else {
		di.acl_mtu  = hdev->le_mtu;
		di.acl_pkts = hdev->le_pkts;
		di.sco_mtu  = 0;
		di.sco_pkts = 0;
	}
	di.link_policy = hdev->link_policy;
	di.link_mode   = hdev->link_mode;

	memcpy(&di.stat, &hdev->stat, sizeof(di.stat));
	memcpy(&di.features, &hdev->features, sizeof(di.features));

	if (copy_to_user(arg, &di, sizeof(di)))
		err = -EFAULT;

	hci_dev_put(hdev);

	return err;
}

/* ---- Interface to HCI drivers ---- */

static int hci_rfkill_set_block(void *data, bool blocked)
{
	struct hci_dev *hdev = data;

	BT_DBG("%p name %s blocked %d", hdev, hdev->name, blocked);

	if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL))
		return -EBUSY;

<<<<<<< HEAD
	hci_dev_do_close(hdev, 0);
=======
	if (blocked) {
		hci_dev_set_flag(hdev, HCI_RFKILLED);
		if (!hci_dev_test_flag(hdev, HCI_SETUP) &&
		    !hci_dev_test_flag(hdev, HCI_CONFIG))
			hci_dev_do_close(hdev);
	} else {
		hci_dev_clear_flag(hdev, HCI_RFKILLED);
	}
>>>>>>> android-4.9

	return 0;
}

static const struct rfkill_ops hci_rfkill_ops = {
	.set_block = hci_rfkill_set_block,
};

<<<<<<< HEAD
/* Alloc HCI device */
struct hci_dev *hci_alloc_dev(void)
{
	struct hci_dev *hdev;

	hdev = kzalloc(sizeof(struct hci_dev), GFP_KERNEL);
	if (!hdev)
		return NULL;

	skb_queue_head_init(&hdev->driver_init);

	return hdev;
}
EXPORT_SYMBOL(hci_alloc_dev);

/* Free HCI device */
void hci_free_dev(struct hci_dev *hdev)
{
	skb_queue_purge(&hdev->driver_init);

	/* will free via device release */
	put_device(&hdev->dev);
}
EXPORT_SYMBOL(hci_free_dev);

=======
>>>>>>> android-4.9
static void hci_power_on(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev, power_on);
	int err;

	BT_DBG("%s", hdev->name);

<<<<<<< HEAD
	err = hci_dev_open(hdev->id);
	if (err && err != -EALREADY)
=======
	if (test_bit(HCI_UP, &hdev->flags) &&
	    hci_dev_test_flag(hdev, HCI_MGMT) &&
	    hci_dev_test_and_clear_flag(hdev, HCI_AUTO_OFF)) {
		cancel_delayed_work(&hdev->power_off);
		hci_req_sync_lock(hdev);
		err = __hci_req_hci_power_on(hdev);
		hci_req_sync_unlock(hdev);
		mgmt_power_on(hdev, err);
		return;
	}

	err = hci_dev_do_open(hdev);
	if (err < 0) {
		hci_dev_lock(hdev);
		mgmt_set_powered_failed(hdev, err);
		hci_dev_unlock(hdev);
>>>>>>> android-4.9
		return;
	}

<<<<<<< HEAD
	if (test_bit(HCI_AUTO_OFF, &hdev->flags) &&
				hdev->dev_type == HCI_BREDR)
		mod_timer(&hdev->off_timer,
				jiffies + msecs_to_jiffies(AUTO_OFF_TIMEOUT));

	if (test_and_clear_bit(HCI_SETUP, &hdev->flags) &&
				hdev->dev_type == HCI_BREDR)
		mgmt_index_added(hdev->id);
=======
	/* During the HCI setup phase, a few error conditions are
	 * ignored and they need to be checked now. If they are still
	 * valid, it is important to turn the device back off.
	 */
	if (hci_dev_test_flag(hdev, HCI_RFKILLED) ||
	    hci_dev_test_flag(hdev, HCI_UNCONFIGURED) ||
	    (hdev->dev_type == HCI_PRIMARY &&
	     !bacmp(&hdev->bdaddr, BDADDR_ANY) &&
	     !bacmp(&hdev->static_addr, BDADDR_ANY))) {
		hci_dev_clear_flag(hdev, HCI_AUTO_OFF);
		hci_dev_do_close(hdev);
	} else if (hci_dev_test_flag(hdev, HCI_AUTO_OFF)) {
		queue_delayed_work(hdev->req_workqueue, &hdev->power_off,
				   HCI_AUTO_OFF_TIMEOUT);
	}

	if (hci_dev_test_and_clear_flag(hdev, HCI_SETUP)) {
		/* For unconfigured devices, set the HCI_RAW flag
		 * so that userspace can easily identify them.
		 */
		if (hci_dev_test_flag(hdev, HCI_UNCONFIGURED))
			set_bit(HCI_RAW, &hdev->flags);

		/* For fully configured devices, this will send
		 * the Index Added event. For unconfigured devices,
		 * it will send Unconfigued Index Added event.
		 *
		 * Devices with HCI_QUIRK_RAW_DEVICE are ignored
		 * and no event will be send.
		 */
		mgmt_index_added(hdev);
	} else if (hci_dev_test_and_clear_flag(hdev, HCI_CONFIG)) {
		/* When the controller is now configured, then it
		 * is important to clear the HCI_RAW flag.
		 */
		if (!hci_dev_test_flag(hdev, HCI_UNCONFIGURED))
			clear_bit(HCI_RAW, &hdev->flags);

		/* Powering on the controller with HCI_CONFIG set only
		 * happens with the transition from unconfigured to
		 * configured. This will send the Index Added event.
		 */
		mgmt_index_added(hdev);
	}
>>>>>>> android-4.9
}

static void hci_power_off(struct work_struct *work)
{
<<<<<<< HEAD
	struct hci_dev *hdev = container_of(work, struct hci_dev, power_off);
=======
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    power_off.work);
>>>>>>> android-4.9

	BT_DBG("%s", hdev->name);

	hci_dev_close(hdev->id);
}

<<<<<<< HEAD
static void hci_auto_off(unsigned long data)
{
	struct hci_dev *hdev = (struct hci_dev *) data;

	BT_DBG("%s", hdev->name);

	clear_bit(HCI_AUTO_OFF, &hdev->flags);

	queue_work(hdev->workqueue, &hdev->power_off);
}

void hci_del_off_timer(struct hci_dev *hdev)
{
	BT_DBG("%s", hdev->name);

	clear_bit(HCI_AUTO_OFF, &hdev->flags);
	del_timer(&hdev->off_timer);
=======
static void hci_error_reset(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev, error_reset);

	BT_DBG("%s", hdev->name);

	if (hdev->hw_error)
		hdev->hw_error(hdev, hdev->hw_error_code);
	else
		BT_ERR("%s hardware error 0x%2.2x", hdev->name,
		       hdev->hw_error_code);

	if (hci_dev_do_close(hdev))
		return;

	hci_dev_do_open(hdev);
>>>>>>> android-4.9
}

void hci_uuids_clear(struct hci_dev *hdev)
{
	struct bt_uuid *uuid, *tmp;

	list_for_each_entry_safe(uuid, tmp, &hdev->uuids, list) {
		list_del(&uuid->list);
		kfree(uuid);
	}
}

void hci_link_keys_clear(struct hci_dev *hdev)
{
	struct link_key *key;

	list_for_each_entry_rcu(key, &hdev->link_keys, list) {
		list_del_rcu(&key->list);
		kfree_rcu(key, rcu);
	}
}

void hci_smp_ltks_clear(struct hci_dev *hdev)
{
	struct smp_ltk *k;

	list_for_each_entry_rcu(k, &hdev->long_term_keys, list) {
		list_del_rcu(&k->list);
		kfree_rcu(k, rcu);
	}
}

<<<<<<< HEAD
struct link_key *hci_find_link_key(struct hci_dev *hdev, bdaddr_t *bdaddr)
{
	struct list_head *p;

	list_for_each(p, &hdev->link_keys) {
		struct link_key *k;
=======
void hci_smp_irks_clear(struct hci_dev *hdev)
{
	struct smp_irk *k;

	list_for_each_entry_rcu(k, &hdev->identity_resolving_keys, list) {
		list_del_rcu(&k->list);
		kfree_rcu(k, rcu);
	}
}
>>>>>>> android-4.9

		k = list_entry(p, struct link_key, list);

<<<<<<< HEAD
		if (bacmp(bdaddr, &k->bdaddr) == 0)
			return k;
	}
=======
	rcu_read_lock();
	list_for_each_entry_rcu(k, &hdev->link_keys, list) {
		if (bacmp(bdaddr, &k->bdaddr) == 0) {
			rcu_read_unlock();
			return k;
		}
	}
	rcu_read_unlock();
>>>>>>> android-4.9

	return NULL;
}

<<<<<<< HEAD
struct link_key *hci_find_ltk(struct hci_dev *hdev, __le16 ediv, u8 rand[8])
{
	struct list_head *p;
=======
static bool hci_persistent_key(struct hci_dev *hdev, struct hci_conn *conn,
			       u8 key_type, u8 old_key_type)
{
	/* Legacy key */
	if (key_type < 0x03)
		return true;

	/* Debug keys are insecure so don't store them persistently */
	if (key_type == HCI_LK_DEBUG_COMBINATION)
		return false;

	/* Changed combination key and there's no previous one */
	if (key_type == HCI_LK_CHANGED_COMBINATION && old_key_type == 0xff)
		return false;

	/* Security mode 3 case */
	if (!conn)
		return true;

	/* BR/EDR key derived using SC from an LE link */
	if (conn->type == LE_LINK)
		return true;

	/* Neither local nor remote side had no-bonding as requirement */
	if (conn->auth_type > 0x01 && conn->remote_auth > 0x01)
		return true;
>>>>>>> android-4.9

	list_for_each(p, &hdev->link_keys) {
		struct link_key *k;
		struct key_master_id *id;

		k = list_entry(p, struct link_key, list);

<<<<<<< HEAD
		if (k->key_type != KEY_TYPE_LTK)
			continue;

		if (k->dlen != sizeof(*id))
			continue;

		id = (void *) &k->data;
		if (id->ediv == ediv &&
				(memcmp(rand, id->rand, sizeof(id->rand)) == 0))
			return k;
=======
	/* If none of the above criteria match, then don't store the key
	 * persistently */
	return false;
}

static u8 ltk_role(u8 type)
{
	if (type == SMP_LTK)
		return HCI_ROLE_MASTER;

	return HCI_ROLE_SLAVE;
}

struct smp_ltk *hci_find_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			     u8 addr_type, u8 role)
{
	struct smp_ltk *k;

	rcu_read_lock();
	list_for_each_entry_rcu(k, &hdev->long_term_keys, list) {
		if (addr_type != k->bdaddr_type || bacmp(bdaddr, &k->bdaddr))
			continue;

		if (smp_ltk_is_sc(k) || ltk_role(k->type) == role) {
			rcu_read_unlock();
			return k;
		}
	}
	rcu_read_unlock();

	return NULL;
}

struct smp_irk *hci_find_irk_by_rpa(struct hci_dev *hdev, bdaddr_t *rpa)
{
	struct smp_irk *irk;

	rcu_read_lock();
	list_for_each_entry_rcu(irk, &hdev->identity_resolving_keys, list) {
		if (!bacmp(&irk->rpa, rpa)) {
			rcu_read_unlock();
			return irk;
		}
	}

	list_for_each_entry_rcu(irk, &hdev->identity_resolving_keys, list) {
		if (smp_irk_matches(hdev, irk->val, rpa)) {
			bacpy(&irk->rpa, rpa);
			rcu_read_unlock();
			return irk;
		}
>>>>>>> android-4.9
	}
	rcu_read_unlock();

	return NULL;
}

<<<<<<< HEAD
struct link_key *hci_find_link_key_type(struct hci_dev *hdev,
					bdaddr_t *bdaddr, u8 type)
{
	struct list_head *p;

	list_for_each(p, &hdev->link_keys) {
		struct link_key *k;

		k = list_entry(p, struct link_key, list);

		if ((k->key_type == type) && (bacmp(bdaddr, &k->bdaddr) == 0))
			return k;
	}

	return NULL;
}
EXPORT_SYMBOL(hci_find_link_key_type);

int hci_add_link_key(struct hci_dev *hdev, int new_key, bdaddr_t *bdaddr,
						u8 *val, u8 type, u8 pin_len)
=======
struct smp_irk *hci_find_irk_by_addr(struct hci_dev *hdev, bdaddr_t *bdaddr,
				     u8 addr_type)
{
	struct smp_irk *irk;

	/* Identity Address must be public or static random */
	if (addr_type == ADDR_LE_DEV_RANDOM && (bdaddr->b[5] & 0xc0) != 0xc0)
		return NULL;

	rcu_read_lock();
	list_for_each_entry_rcu(irk, &hdev->identity_resolving_keys, list) {
		if (addr_type == irk->addr_type &&
		    bacmp(bdaddr, &irk->bdaddr) == 0) {
			rcu_read_unlock();
			return irk;
		}
	}
	rcu_read_unlock();

	return NULL;
}

struct link_key *hci_add_link_key(struct hci_dev *hdev, struct hci_conn *conn,
				  bdaddr_t *bdaddr, u8 *val, u8 type,
				  u8 pin_len, bool *persistent)
>>>>>>> android-4.9
{
	struct link_key *key, *old_key;
	struct hci_conn *conn;
	u8 old_key_type;
<<<<<<< HEAD
	u8 bonded = 0;
=======
>>>>>>> android-4.9

	old_key = hci_find_link_key(hdev, bdaddr);
	if (old_key) {
		old_key_type = old_key->key_type;
		key = old_key;
	} else {
<<<<<<< HEAD
		old_key_type = 0xff;
		key = kzalloc(sizeof(*key), GFP_ATOMIC);
=======
		old_key_type = conn ? conn->key_type : 0xff;
		key = kzalloc(sizeof(*key), GFP_KERNEL);
>>>>>>> android-4.9
		if (!key)
			return NULL;
		list_add_rcu(&key->list, &hdev->link_keys);
	}

	BT_DBG("%s key for %pMR type %u", hdev->name, bdaddr, type);

<<<<<<< HEAD
	bacpy(&key->bdaddr, bdaddr);
	memcpy(key->val, val, 16);
	key->auth = 0x01;
	key->key_type = type;
	key->pin_len = pin_len;

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, bdaddr);
	/* Store the link key persistently if one of the following is true:
	 * 1. the remote side is using dedicated bonding since in that case
	 *    also the local requirements are set to dedicated bonding
	 * 2. the local side had dedicated bonding as a requirement
	 * 3. this is a legacy link key
	 * 4. this is a changed combination key and there was a previously
	 *    stored one
	 * If none of the above match only keep the link key around for
	 * this connection and set the temporary flag for the device.
	*/

	if (conn) {
		if ((conn->remote_auth > 0x01) ||
			(conn->auth_initiator && conn->auth_type > 0x01) ||
			(key->key_type < 0x03) ||
			(key->key_type == 0x06 && old_key_type != 0xff))
			bonded = 1;
	}

	if (new_key)
		mgmt_new_key(hdev->id, key, bonded);

	if (type == 0x06)
		key->key_type = old_key_type;

	return 0;
}

int hci_add_ltk(struct hci_dev *hdev, int new_key, bdaddr_t *bdaddr,
			u8 addr_type, u8 key_size, u8 auth,
			__le16 ediv, u8 rand[8], u8 ltk[16])
{
	struct link_key *key, *old_key;
	struct key_master_id *id;

	BT_DBG("%s Auth: %2.2X addr %s type: %d", hdev->name, auth,
						batostr(bdaddr), addr_type);

	old_key = hci_find_link_key_type(hdev, bdaddr, KEY_TYPE_LTK);
	if (old_key) {
		key = old_key;
	} else {
		key = kzalloc(sizeof(*key) + sizeof(*id), GFP_ATOMIC);
		if (!key)
			return -ENOMEM;
		list_add(&key->list, &hdev->link_keys);
=======
	/* Some buggy controller combinations generate a changed
	 * combination key for legacy pairing even when there's no
	 * previous key */
	if (type == HCI_LK_CHANGED_COMBINATION &&
	    (!conn || conn->remote_auth == 0xff) && old_key_type == 0xff) {
		type = HCI_LK_COMBINATION;
		if (conn)
			conn->key_type = type;
	}

	bacpy(&key->bdaddr, bdaddr);
	memcpy(key->val, val, HCI_LINK_KEY_SIZE);
	key->pin_len = pin_len;

	if (type == HCI_LK_CHANGED_COMBINATION)
		key->type = old_key_type;
	else
		key->type = type;

	if (persistent)
		*persistent = hci_persistent_key(hdev, conn, type,
						 old_key_type);

	return key;
}

struct smp_ltk *hci_add_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 addr_type, u8 type, u8 authenticated,
			    u8 tk[16], u8 enc_size, __le16 ediv, __le64 rand)
{
	struct smp_ltk *key, *old_key;
	u8 role = ltk_role(type);

	old_key = hci_find_ltk(hdev, bdaddr, addr_type, role);
	if (old_key)
		key = old_key;
	else {
		key = kzalloc(sizeof(*key), GFP_KERNEL);
		if (!key)
			return NULL;
		list_add_rcu(&key->list, &hdev->long_term_keys);
>>>>>>> android-4.9
	}

	key->dlen = sizeof(*id);

	bacpy(&key->bdaddr, bdaddr);
<<<<<<< HEAD
	key->addr_type = addr_type;
	memcpy(key->val, ltk, sizeof(key->val));
	key->key_type = KEY_TYPE_LTK;
	key->pin_len = key_size;
	key->auth = auth;

	id = (void *) &key->data;
	id->ediv = ediv;
	memcpy(id->rand, rand, sizeof(id->rand));

	if (new_key)
		mgmt_new_key(hdev->id, key, auth & 0x01);
=======
	key->bdaddr_type = addr_type;
	memcpy(key->val, tk, sizeof(key->val));
	key->authenticated = authenticated;
	key->ediv = ediv;
	key->rand = rand;
	key->enc_size = enc_size;
	key->type = type;

	return key;
}

struct smp_irk *hci_add_irk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 addr_type, u8 val[16], bdaddr_t *rpa)
{
	struct smp_irk *irk;

	irk = hci_find_irk_by_addr(hdev, bdaddr, addr_type);
	if (!irk) {
		irk = kzalloc(sizeof(*irk), GFP_KERNEL);
		if (!irk)
			return NULL;
>>>>>>> android-4.9

		bacpy(&irk->bdaddr, bdaddr);
		irk->addr_type = addr_type;

		list_add_rcu(&irk->list, &hdev->identity_resolving_keys);
	}

	memcpy(irk->val, val, 16);
	bacpy(&irk->rpa, rpa);

	return irk;
}

int hci_remove_link_key(struct hci_dev *hdev, bdaddr_t *bdaddr)
{
	struct link_key *key;

	key = hci_find_link_key(hdev, bdaddr);
	if (!key)
		return -ENOENT;

	BT_DBG("%s removing %pMR", hdev->name, bdaddr);

	list_del_rcu(&key->list);
	kfree_rcu(key, rcu);

	return 0;
}

<<<<<<< HEAD
=======
int hci_remove_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 bdaddr_type)
{
	struct smp_ltk *k;
	int removed = 0;

	list_for_each_entry_rcu(k, &hdev->long_term_keys, list) {
		if (bacmp(bdaddr, &k->bdaddr) || k->bdaddr_type != bdaddr_type)
			continue;

		BT_DBG("%s removing %pMR", hdev->name, bdaddr);

		list_del_rcu(&k->list);
		kfree_rcu(k, rcu);
		removed++;
	}

	return removed ? 0 : -ENOENT;
}

void hci_remove_irk(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 addr_type)
{
	struct smp_irk *k;

	list_for_each_entry_rcu(k, &hdev->identity_resolving_keys, list) {
		if (bacmp(bdaddr, &k->bdaddr) || k->addr_type != addr_type)
			continue;

		BT_DBG("%s removing %pMR", hdev->name, bdaddr);

		list_del_rcu(&k->list);
		kfree_rcu(k, rcu);
	}
}

bool hci_bdaddr_is_paired(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 type)
{
	struct smp_ltk *k;
	struct smp_irk *irk;
	u8 addr_type;

	if (type == BDADDR_BREDR) {
		if (hci_find_link_key(hdev, bdaddr))
			return true;
		return false;
	}

	/* Convert to HCI addr type which struct smp_ltk uses */
	if (type == BDADDR_LE_PUBLIC)
		addr_type = ADDR_LE_DEV_PUBLIC;
	else
		addr_type = ADDR_LE_DEV_RANDOM;

	irk = hci_get_irk(hdev, bdaddr, addr_type);
	if (irk) {
		bdaddr = &irk->bdaddr;
		addr_type = irk->addr_type;
	}

	rcu_read_lock();
	list_for_each_entry_rcu(k, &hdev->long_term_keys, list) {
		if (k->bdaddr_type == addr_type && !bacmp(bdaddr, &k->bdaddr)) {
			rcu_read_unlock();
			return true;
		}
	}
	rcu_read_unlock();

	return false;
}

>>>>>>> android-4.9
/* HCI command timer function */
static void hci_cmd_timeout(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    cmd_timer.work);

	if (hdev->sent_cmd) {
		struct hci_command_hdr *sent = (void *) hdev->sent_cmd->data;
		u16 opcode = __le16_to_cpu(sent->opcode);

		BT_ERR("%s command 0x%4.4x tx timeout", hdev->name, opcode);
	} else {
		BT_ERR("%s command tx timeout", hdev->name);
	}

	atomic_set(&hdev->cmd_cnt, 1);
	clear_bit(HCI_RESET, &hdev->flags);
	tasklet_schedule(&hdev->cmd_task);
}

struct oob_data *hci_find_remote_oob_data(struct hci_dev *hdev,
<<<<<<< HEAD
							bdaddr_t *bdaddr)
=======
					  bdaddr_t *bdaddr, u8 bdaddr_type)
>>>>>>> android-4.9
{
	struct oob_data *data;

	list_for_each_entry(data, &hdev->remote_oob_data, list) {
		if (bacmp(bdaddr, &data->bdaddr) != 0)
			continue;
		if (data->bdaddr_type != bdaddr_type)
			continue;
		return data;
	}

	return NULL;
}

int hci_remove_remote_oob_data(struct hci_dev *hdev, bdaddr_t *bdaddr,
			       u8 bdaddr_type)
{
	struct oob_data *data;

	data = hci_find_remote_oob_data(hdev, bdaddr, bdaddr_type);
	if (!data)
		return -ENOENT;

	BT_DBG("%s removing %pMR (%u)", hdev->name, bdaddr, bdaddr_type);

	list_del(&data->list);
	kfree(data);

	return 0;
}

void hci_remote_oob_data_clear(struct hci_dev *hdev)
{
	struct oob_data *data, *n;

	list_for_each_entry_safe(data, n, &hdev->remote_oob_data, list) {
		list_del(&data->list);
		kfree(data);
	}
}

<<<<<<< HEAD
static void hci_adv_clear(unsigned long arg)
=======
int hci_add_remote_oob_data(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 bdaddr_type, u8 *hash192, u8 *rand192,
			    u8 *hash256, u8 *rand256)
>>>>>>> android-4.9
{
	struct hci_dev *hdev = (void *) arg;

<<<<<<< HEAD
	hci_adv_entries_clear(hdev);
}

int hci_adv_entries_clear(struct hci_dev *hdev)
{
	struct list_head *p, *n;

	BT_DBG("");
	write_lock_bh(&hdev->adv_entries_lock);

	list_for_each_safe(p, n, &hdev->adv_entries) {
		struct adv_entry *entry;

		entry = list_entry(p, struct adv_entry, list);

		list_del(p);
		kfree(entry);
	}

	write_unlock_bh(&hdev->adv_entries_lock);

	return 0;
}

struct adv_entry *hci_find_adv_entry(struct hci_dev *hdev, bdaddr_t *bdaddr)
{
	struct list_head *p;
	struct adv_entry *res = NULL;

	BT_DBG("");
	read_lock_bh(&hdev->adv_entries_lock);

	list_for_each(p, &hdev->adv_entries) {
		struct adv_entry *entry;

		entry = list_entry(p, struct adv_entry, list);

		if (bacmp(bdaddr, &entry->bdaddr) == 0) {
			res = entry;
			goto out;
		}
	}
out:
	read_unlock_bh(&hdev->adv_entries_lock);
	return res;
}

static inline int is_connectable_adv(u8 evt_type)
{
	if (evt_type == ADV_IND || evt_type == ADV_DIRECT_IND)
		return 1;

	return 0;
}

int hci_add_remote_oob_data(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 *hash,
								u8 *randomizer)
{
	struct oob_data *data;

	data = hci_find_remote_oob_data(hdev, bdaddr);

	if (!data) {
		data = kmalloc(sizeof(*data), GFP_ATOMIC);
		if (!data)
			return -ENOMEM;

		bacpy(&data->bdaddr, bdaddr);
		list_add(&data->list, &hdev->remote_oob_data);
	}

	memcpy(data->hash, hash, sizeof(data->hash));
	memcpy(data->randomizer, randomizer, sizeof(data->randomizer));

	BT_DBG("%s for %s", hdev->name, batostr(bdaddr));
=======
	data = hci_find_remote_oob_data(hdev, bdaddr, bdaddr_type);
	if (!data) {
		data = kmalloc(sizeof(*data), GFP_KERNEL);
		if (!data)
			return -ENOMEM;

		bacpy(&data->bdaddr, bdaddr);
		data->bdaddr_type = bdaddr_type;
		list_add(&data->list, &hdev->remote_oob_data);
	}

	if (hash192 && rand192) {
		memcpy(data->hash192, hash192, sizeof(data->hash192));
		memcpy(data->rand192, rand192, sizeof(data->rand192));
		if (hash256 && rand256)
			data->present = 0x03;
	} else {
		memset(data->hash192, 0, sizeof(data->hash192));
		memset(data->rand192, 0, sizeof(data->rand192));
		if (hash256 && rand256)
			data->present = 0x02;
		else
			data->present = 0x00;
	}

	if (hash256 && rand256) {
		memcpy(data->hash256, hash256, sizeof(data->hash256));
		memcpy(data->rand256, rand256, sizeof(data->rand256));
	} else {
		memset(data->hash256, 0, sizeof(data->hash256));
		memset(data->rand256, 0, sizeof(data->rand256));
		if (hash192 && rand192)
			data->present = 0x01;
	}

	BT_DBG("%s for %pMR", hdev->name, bdaddr);

	return 0;
}

/* This function requires the caller holds hdev->lock */
struct adv_info *hci_find_adv_instance(struct hci_dev *hdev, u8 instance)
{
	struct adv_info *adv_instance;

	list_for_each_entry(adv_instance, &hdev->adv_instances, list) {
		if (adv_instance->instance == instance)
			return adv_instance;
	}

	return NULL;
}

/* This function requires the caller holds hdev->lock */
struct adv_info *hci_get_next_instance(struct hci_dev *hdev, u8 instance)
{
	struct adv_info *cur_instance;

	cur_instance = hci_find_adv_instance(hdev, instance);
	if (!cur_instance)
		return NULL;

	if (cur_instance == list_last_entry(&hdev->adv_instances,
					    struct adv_info, list))
		return list_first_entry(&hdev->adv_instances,
						 struct adv_info, list);
	else
		return list_next_entry(cur_instance, list);
}

/* This function requires the caller holds hdev->lock */
int hci_remove_adv_instance(struct hci_dev *hdev, u8 instance)
{
	struct adv_info *adv_instance;

	adv_instance = hci_find_adv_instance(hdev, instance);
	if (!adv_instance)
		return -ENOENT;

	BT_DBG("%s removing %dMR", hdev->name, instance);

	if (hdev->cur_adv_instance == instance) {
		if (hdev->adv_instance_timeout) {
			cancel_delayed_work(&hdev->adv_instance_expire);
			hdev->adv_instance_timeout = 0;
		}
		hdev->cur_adv_instance = 0x00;
	}

	list_del(&adv_instance->list);
	kfree(adv_instance);

	hdev->adv_instance_cnt--;

	return 0;
}

/* This function requires the caller holds hdev->lock */
void hci_adv_instances_clear(struct hci_dev *hdev)
{
	struct adv_info *adv_instance, *n;

	if (hdev->adv_instance_timeout) {
		cancel_delayed_work(&hdev->adv_instance_expire);
		hdev->adv_instance_timeout = 0;
	}

	list_for_each_entry_safe(adv_instance, n, &hdev->adv_instances, list) {
		list_del(&adv_instance->list);
		kfree(adv_instance);
	}

	hdev->adv_instance_cnt = 0;
	hdev->cur_adv_instance = 0x00;
}

/* This function requires the caller holds hdev->lock */
int hci_add_adv_instance(struct hci_dev *hdev, u8 instance, u32 flags,
			 u16 adv_data_len, u8 *adv_data,
			 u16 scan_rsp_len, u8 *scan_rsp_data,
			 u16 timeout, u16 duration)
{
	struct adv_info *adv_instance;

	adv_instance = hci_find_adv_instance(hdev, instance);
	if (adv_instance) {
		memset(adv_instance->adv_data, 0,
		       sizeof(adv_instance->adv_data));
		memset(adv_instance->scan_rsp_data, 0,
		       sizeof(adv_instance->scan_rsp_data));
	} else {
		if (hdev->adv_instance_cnt >= HCI_MAX_ADV_INSTANCES ||
		    instance < 1 || instance > HCI_MAX_ADV_INSTANCES)
			return -EOVERFLOW;

		adv_instance = kzalloc(sizeof(*adv_instance), GFP_KERNEL);
		if (!adv_instance)
			return -ENOMEM;

		adv_instance->pending = true;
		adv_instance->instance = instance;
		list_add(&adv_instance->list, &hdev->adv_instances);
		hdev->adv_instance_cnt++;
	}

	adv_instance->flags = flags;
	adv_instance->adv_data_len = adv_data_len;
	adv_instance->scan_rsp_len = scan_rsp_len;

	if (adv_data_len)
		memcpy(adv_instance->adv_data, adv_data, adv_data_len);

	if (scan_rsp_len)
		memcpy(adv_instance->scan_rsp_data,
		       scan_rsp_data, scan_rsp_len);

	adv_instance->timeout = timeout;
	adv_instance->remaining_time = timeout;

	if (duration == 0)
		adv_instance->duration = HCI_DEFAULT_ADV_DURATION;
	else
		adv_instance->duration = duration;

	BT_DBG("%s for %dMR", hdev->name, instance);
>>>>>>> android-4.9

	return 0;
}

<<<<<<< HEAD
int hci_add_adv_entry(struct hci_dev *hdev,
					struct hci_ev_le_advertising_info *ev)
{
	struct adv_entry *entry;
	u8 flags = 0;
	int i;

	BT_DBG("");

	if (!is_connectable_adv(ev->evt_type))
		return -EINVAL;

	if (ev->data && ev->length) {
		for (i = 0; (i + 2) < ev->length; i++)
			if (ev->data[i+1] == 0x01) {
				flags = ev->data[i+2];
				BT_DBG("flags: %2.2x", flags);
				break;
			} else {
				i += ev->data[i];
			}
	}

	entry = hci_find_adv_entry(hdev, &ev->bdaddr);
	/* Only new entries should be added to adv_entries. So, if
	 * bdaddr was found, don't add it. */
	if (entry) {
		entry->flags = flags;
		return 0;
	}
=======
struct bdaddr_list *hci_bdaddr_list_lookup(struct list_head *bdaddr_list,
					 bdaddr_t *bdaddr, u8 type)
{
	struct bdaddr_list *b;

	list_for_each_entry(b, bdaddr_list, list) {
		if (!bacmp(&b->bdaddr, bdaddr) && b->bdaddr_type == type)
			return b;
	}

	return NULL;
}

void hci_bdaddr_list_clear(struct list_head *bdaddr_list)
{
	struct bdaddr_list *b, *n;

	list_for_each_entry_safe(b, n, bdaddr_list, list) {
		list_del(&b->list);
		kfree(b);
	}
}

int hci_bdaddr_list_add(struct list_head *list, bdaddr_t *bdaddr, u8 type)
{
	struct bdaddr_list *entry;

	if (!bacmp(bdaddr, BDADDR_ANY))
		return -EBADF;

	if (hci_bdaddr_list_lookup(list, bdaddr, type))
		return -EEXIST;
>>>>>>> android-4.9

	entry = kzalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry)
		return -ENOMEM;

<<<<<<< HEAD
	bacpy(&entry->bdaddr, &ev->bdaddr);
	entry->bdaddr_type = ev->bdaddr_type;
	entry->flags = flags;

	write_lock(&hdev->adv_entries_lock);
	list_add(&entry->list, &hdev->adv_entries);
	write_unlock(&hdev->adv_entries_lock);
=======
	bacpy(&entry->bdaddr, bdaddr);
	entry->bdaddr_type = type;

	list_add(&entry->list, list);

	return 0;
}

int hci_bdaddr_list_del(struct list_head *list, bdaddr_t *bdaddr, u8 type)
{
	struct bdaddr_list *entry;

	if (!bacmp(bdaddr, BDADDR_ANY)) {
		hci_bdaddr_list_clear(list);
		return 0;
	}

	entry = hci_bdaddr_list_lookup(list, bdaddr, type);
	if (!entry)
		return -ENOENT;

	list_del(&entry->list);
	kfree(entry);

	return 0;
}

/* This function requires the caller holds hdev->lock */
struct hci_conn_params *hci_conn_params_lookup(struct hci_dev *hdev,
					       bdaddr_t *addr, u8 addr_type)
{
	struct hci_conn_params *params;

	list_for_each_entry(params, &hdev->le_conn_params, list) {
		if (bacmp(&params->addr, addr) == 0 &&
		    params->addr_type == addr_type) {
			return params;
		}
	}

	return NULL;
}

/* This function requires the caller holds hdev->lock */
struct hci_conn_params *hci_pend_le_action_lookup(struct list_head *list,
						  bdaddr_t *addr, u8 addr_type)
{
	struct hci_conn_params *param;

	list_for_each_entry(param, list, action) {
		if (bacmp(&param->addr, addr) == 0 &&
		    param->addr_type == addr_type)
			return param;
	}

	return NULL;
}

/* This function requires the caller holds hdev->lock */
struct hci_conn_params *hci_conn_params_add(struct hci_dev *hdev,
					    bdaddr_t *addr, u8 addr_type)
{
	struct hci_conn_params *params;

	params = hci_conn_params_lookup(hdev, addr, addr_type);
	if (params)
		return params;

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params) {
		BT_ERR("Out of memory");
		return NULL;
	}

	bacpy(&params->addr, addr);
	params->addr_type = addr_type;

	list_add(&params->list, &hdev->le_conn_params);
	INIT_LIST_HEAD(&params->action);

	params->conn_min_interval = hdev->le_conn_min_interval;
	params->conn_max_interval = hdev->le_conn_max_interval;
	params->conn_latency = hdev->le_conn_latency;
	params->supervision_timeout = hdev->le_supv_timeout;
	params->auto_connect = HCI_AUTO_CONN_DISABLED;

	BT_DBG("addr %pMR (type %u)", addr, addr_type);
>>>>>>> android-4.9

	return params;
}

<<<<<<< HEAD
static struct crypto_blkcipher *alloc_cypher(void)
{
	if (enable_smp)
		return crypto_alloc_blkcipher("ecb(aes)", 0, CRYPTO_ALG_ASYNC);

	return ERR_PTR(-ENOTSUPP);
=======
static void hci_conn_params_free(struct hci_conn_params *params)
{
	if (params->conn) {
		hci_conn_drop(params->conn);
		hci_conn_put(params->conn);
	}

	list_del(&params->action);
	list_del(&params->list);
	kfree(params);
}

/* This function requires the caller holds hdev->lock */
void hci_conn_params_del(struct hci_dev *hdev, bdaddr_t *addr, u8 addr_type)
{
	struct hci_conn_params *params;

	params = hci_conn_params_lookup(hdev, addr, addr_type);
	if (!params)
		return;

	hci_conn_params_free(params);

	hci_update_background_scan(hdev);

	BT_DBG("addr %pMR (type %u)", addr, addr_type);
}

/* This function requires the caller holds hdev->lock */
void hci_conn_params_clear_disabled(struct hci_dev *hdev)
{
	struct hci_conn_params *params, *tmp;

	list_for_each_entry_safe(params, tmp, &hdev->le_conn_params, list) {
		if (params->auto_connect != HCI_AUTO_CONN_DISABLED)
			continue;

		/* If trying to estabilish one time connection to disabled
		 * device, leave the params, but mark them as just once.
		 */
		if (params->explicit_connect) {
			params->auto_connect = HCI_AUTO_CONN_EXPLICIT;
			continue;
		}

		list_del(&params->list);
		kfree(params);
	}

	BT_DBG("All LE disabled connection parameters were removed");
>>>>>>> android-4.9
}

/* This function requires the caller holds hdev->lock */
static void hci_conn_params_clear_all(struct hci_dev *hdev)
{
<<<<<<< HEAD
	struct list_head *head = &hci_dev_list, *p;
	int i, id;

	BT_DBG("%p name %s bus %d owner %p", hdev, hdev->name,
						hdev->bus, hdev->owner);

	if (!hdev->open || !hdev->close || !hdev->destruct)
		return -EINVAL;

	id = (hdev->dev_type == HCI_BREDR) ? 0 : 1;

	write_lock_bh(&hci_dev_list_lock);
=======
	struct hci_conn_params *params, *tmp;

	list_for_each_entry_safe(params, tmp, &hdev->le_conn_params, list)
		hci_conn_params_free(params);

	BT_DBG("All LE connection parameters were removed");
}
>>>>>>> android-4.9

/* Copy the Identity Address of the controller.
 *
 * If the controller has a public BD_ADDR, then by default use that one.
 * If this is a LE only controller without a public address, default to
 * the static random address.
 *
 * For debugging purposes it is possible to force controllers with a
 * public address to use the static random address instead.
 *
 * In case BR/EDR has been disabled on a dual-mode controller and
 * userspace has configured a static address, then that address
 * becomes the identity address instead of the public BR/EDR address.
 */
void hci_copy_identity_address(struct hci_dev *hdev, bdaddr_t *bdaddr,
			       u8 *bdaddr_type)
{
	if (hci_dev_test_flag(hdev, HCI_FORCE_STATIC_ADDR) ||
	    !bacmp(&hdev->bdaddr, BDADDR_ANY) ||
	    (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED) &&
	     bacmp(&hdev->static_addr, BDADDR_ANY))) {
		bacpy(bdaddr, &hdev->static_addr);
		*bdaddr_type = ADDR_LE_DEV_RANDOM;
	} else {
		bacpy(bdaddr, &hdev->bdaddr);
		*bdaddr_type = ADDR_LE_DEV_PUBLIC;
	}
}

<<<<<<< HEAD
	sprintf(hdev->name, "hci%d", id);
	hdev->id = id;
	list_add(&hdev->list, head);

	atomic_set(&hdev->refcnt, 1);
	spin_lock_init(&hdev->lock);

	hdev->flags = 0;
=======
/* Alloc HCI device */
struct hci_dev *hci_alloc_dev(void)
{
	struct hci_dev *hdev;

	hdev = kzalloc(sizeof(*hdev), GFP_KERNEL);
	if (!hdev)
		return NULL;

>>>>>>> android-4.9
	hdev->pkt_type  = (HCI_DM1 | HCI_DH1 | HCI_HV1);
	hdev->esco_type = (ESCO_HV1);
	hdev->link_mode = (HCI_LM_ACCEPT);
	hdev->num_iac = 0x01;		/* One IAC support is mandatory */
	hdev->io_capability = 0x03;	/* No Input No Output */
	hdev->manufacturer = 0xffff;	/* Default to internal use */
	hdev->inq_tx_power = HCI_TX_POWER_INVALID;
	hdev->adv_tx_power = HCI_TX_POWER_INVALID;
	hdev->adv_instance_cnt = 0;
	hdev->cur_adv_instance = 0x00;
	hdev->adv_instance_timeout = 0;

	hdev->sniff_max_interval = 800;
	hdev->sniff_min_interval = 80;

<<<<<<< HEAD
	tasklet_init(&hdev->cmd_task, hci_cmd_task, (unsigned long) hdev);
	tasklet_init(&hdev->rx_task, hci_rx_task, (unsigned long) hdev);
	tasklet_init(&hdev->tx_task, hci_tx_task, (unsigned long) hdev);
=======
	hdev->le_adv_channel_map = 0x07;
	hdev->le_adv_min_interval = 0x0800;
	hdev->le_adv_max_interval = 0x0800;
	hdev->le_scan_interval = 0x0060;
	hdev->le_scan_window = 0x0030;
	hdev->le_conn_min_interval = 0x0028;
	hdev->le_conn_max_interval = 0x0038;
	hdev->le_conn_latency = 0x0000;
	hdev->le_supv_timeout = 0x002a;
	hdev->le_def_tx_len = 0x001b;
	hdev->le_def_tx_time = 0x0148;
	hdev->le_max_tx_len = 0x001b;
	hdev->le_max_tx_time = 0x0148;
	hdev->le_max_rx_len = 0x001b;
	hdev->le_max_rx_time = 0x0148;

	hdev->rpa_timeout = HCI_DEFAULT_RPA_TIMEOUT;
	hdev->discov_interleaved_timeout = DISCOV_INTERLEAVED_TIMEOUT;
	hdev->conn_info_min_age = DEFAULT_CONN_INFO_MIN_AGE;
	hdev->conn_info_max_age = DEFAULT_CONN_INFO_MAX_AGE;

	mutex_init(&hdev->lock);
	mutex_init(&hdev->req_lock);

	INIT_LIST_HEAD(&hdev->mgmt_pending);
	INIT_LIST_HEAD(&hdev->blacklist);
	INIT_LIST_HEAD(&hdev->whitelist);
	INIT_LIST_HEAD(&hdev->uuids);
	INIT_LIST_HEAD(&hdev->link_keys);
	INIT_LIST_HEAD(&hdev->long_term_keys);
	INIT_LIST_HEAD(&hdev->identity_resolving_keys);
	INIT_LIST_HEAD(&hdev->remote_oob_data);
	INIT_LIST_HEAD(&hdev->le_white_list);
	INIT_LIST_HEAD(&hdev->le_conn_params);
	INIT_LIST_HEAD(&hdev->pend_le_conns);
	INIT_LIST_HEAD(&hdev->pend_le_reports);
	INIT_LIST_HEAD(&hdev->conn_hash.list);
	INIT_LIST_HEAD(&hdev->adv_instances);

	INIT_WORK(&hdev->rx_work, hci_rx_work);
	INIT_WORK(&hdev->cmd_work, hci_cmd_work);
	INIT_WORK(&hdev->tx_work, hci_tx_work);
	INIT_WORK(&hdev->power_on, hci_power_on);
	INIT_WORK(&hdev->error_reset, hci_error_reset);

	INIT_DELAYED_WORK(&hdev->power_off, hci_power_off);
>>>>>>> android-4.9

	skb_queue_head_init(&hdev->rx_q);
	skb_queue_head_init(&hdev->cmd_q);
	skb_queue_head_init(&hdev->raw_q);

<<<<<<< HEAD
	setup_timer(&hdev->cmd_timer, hci_cmd_timer, (unsigned long) hdev);
	setup_timer(&hdev->disco_timer, mgmt_disco_timeout,
						(unsigned long) hdev);
	setup_timer(&hdev->disco_le_timer, mgmt_disco_le_timeout,
						(unsigned long) hdev);

	for (i = 0; i < NUM_REASSEMBLY; i++)
		hdev->reassembly[i] = NULL;

=======
>>>>>>> android-4.9
	init_waitqueue_head(&hdev->req_wait_q);

<<<<<<< HEAD
	inquiry_cache_init(hdev);

	hci_conn_hash_init(hdev);
	hci_chan_list_init(hdev);

	INIT_LIST_HEAD(&hdev->blacklist);

	INIT_LIST_HEAD(&hdev->uuids);

	INIT_LIST_HEAD(&hdev->link_keys);
=======
	INIT_DELAYED_WORK(&hdev->cmd_timer, hci_cmd_timeout);

	hci_request_setup(hdev);
>>>>>>> android-4.9

	hci_init_sysfs(hdev);
	discovery_init(hdev);

<<<<<<< HEAD
	INIT_LIST_HEAD(&hdev->adv_entries);
	rwlock_init(&hdev->adv_entries_lock);
	setup_timer(&hdev->adv_timer, hci_adv_clear, (unsigned long) hdev);

	INIT_WORK(&hdev->power_on, hci_power_on);
	INIT_WORK(&hdev->power_off, hci_power_off);
	setup_timer(&hdev->off_timer, hci_auto_off, (unsigned long) hdev);
=======
	return hdev;
}
EXPORT_SYMBOL(hci_alloc_dev);

/* Free HCI device */
void hci_free_dev(struct hci_dev *hdev)
{
	/* will free via device release */
	put_device(&hdev->dev);
}
EXPORT_SYMBOL(hci_free_dev);

/* Register HCI device */
int hci_register_dev(struct hci_dev *hdev)
{
	int id, error;
>>>>>>> android-4.9

	if (!hdev->open || !hdev->close || !hdev->send)
		return -EINVAL;

	/* Do not allow HCI_AMP devices to register at index 0,
	 * so the index can be used as the AMP controller ID.
	 */
	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		id = ida_simple_get(&hci_index_ida, 0, 0, GFP_KERNEL);
		break;
	case HCI_AMP:
		id = ida_simple_get(&hci_index_ida, 1, 0, GFP_KERNEL);
		break;
	default:
		return -EINVAL;
	}

<<<<<<< HEAD
	write_unlock_bh(&hci_dev_list_lock);

	hdev->workqueue = create_singlethread_workqueue(hdev->name);
	if (!hdev->workqueue)
		goto nomem;

	hdev->tfm = alloc_cypher();
	if (IS_ERR(hdev->tfm))
		BT_INFO("Failed to load transform for ecb(aes): %ld",
							PTR_ERR(hdev->tfm));

	hci_register_sysfs(hdev);
=======
	if (id < 0)
		return id;

	sprintf(hdev->name, "hci%d", id);
	hdev->id = id;

	BT_DBG("%p name %s bus %d", hdev, hdev->name, hdev->bus);

	hdev->workqueue = alloc_workqueue("%s", WQ_HIGHPRI | WQ_UNBOUND |
					  WQ_MEM_RECLAIM, 1, hdev->name);
	if (!hdev->workqueue) {
		error = -ENOMEM;
		goto err;
	}

	hdev->req_workqueue = alloc_workqueue("%s", WQ_HIGHPRI | WQ_UNBOUND |
					      WQ_MEM_RECLAIM, 1, hdev->name);
	if (!hdev->req_workqueue) {
		destroy_workqueue(hdev->workqueue);
		error = -ENOMEM;
		goto err;
	}

	if (!IS_ERR_OR_NULL(bt_debugfs))
		hdev->debugfs = debugfs_create_dir(hdev->name, bt_debugfs);

	dev_set_name(&hdev->dev, "%s", hdev->name);

	error = device_add(&hdev->dev);
	if (error < 0)
		goto err_wqueue;
>>>>>>> android-4.9

	hci_leds_init(hdev);

	hdev->rfkill = rfkill_alloc(hdev->name, &hdev->dev,
				    RFKILL_TYPE_BLUETOOTH, &hci_rfkill_ops,
				    hdev);
	if (hdev->rfkill) {
		if (rfkill_register(hdev->rfkill) < 0) {
			rfkill_destroy(hdev->rfkill);
			hdev->rfkill = NULL;
		}
	}

<<<<<<< HEAD
	set_bit(HCI_AUTO_OFF, &hdev->flags);
	set_bit(HCI_SETUP, &hdev->flags);
	queue_work(hdev->workqueue, &hdev->power_on);

	hci_notify(hdev, HCI_DEV_REG);
=======
	if (hdev->rfkill && rfkill_blocked(hdev->rfkill))
		hci_dev_set_flag(hdev, HCI_RFKILLED);

	hci_dev_set_flag(hdev, HCI_SETUP);
	hci_dev_set_flag(hdev, HCI_AUTO_OFF);

	if (hdev->dev_type == HCI_PRIMARY) {
		/* Assume BR/EDR support until proven otherwise (such as
		 * through reading supported features during init.
		 */
		hci_dev_set_flag(hdev, HCI_BREDR_ENABLED);
	}

	write_lock(&hci_dev_list_lock);
	list_add(&hdev->list, &hci_dev_list);
	write_unlock(&hci_dev_list_lock);

	/* Devices that are marked for raw-only usage are unconfigured
	 * and should not be included in normal operation.
	 */
	if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
		hci_dev_set_flag(hdev, HCI_UNCONFIGURED);

	hci_sock_dev_event(hdev, HCI_DEV_REG);
	hci_dev_hold(hdev);
>>>>>>> android-4.9

	queue_work(hdev->req_workqueue, &hdev->power_on);

	return id;

<<<<<<< HEAD
nomem:
	write_lock_bh(&hci_dev_list_lock);
	list_del(&hdev->list);
	write_unlock_bh(&hci_dev_list_lock);
=======
err_wqueue:
	destroy_workqueue(hdev->workqueue);
	destroy_workqueue(hdev->req_workqueue);
err:
	ida_simple_remove(&hci_index_ida, hdev->id);
>>>>>>> android-4.9

	return -ENOMEM;
}
EXPORT_SYMBOL(hci_register_dev);

/* Unregister HCI device */
int hci_unregister_dev(struct hci_dev *hdev)
{
	int id;

	BT_DBG("%p name %s bus %d", hdev, hdev->name, hdev->bus);

<<<<<<< HEAD
	write_lock_bh(&hci_dev_list_lock);
=======
	hci_dev_set_flag(hdev, HCI_UNREGISTER);

	id = hdev->id;

	write_lock(&hci_dev_list_lock);
>>>>>>> android-4.9
	list_del(&hdev->list);
	write_unlock_bh(&hci_dev_list_lock);

<<<<<<< HEAD
	hci_dev_do_close(hdev, hdev->bus == HCI_SMD);
=======
	cancel_work_sync(&hdev->power_on);
>>>>>>> android-4.9

	hci_dev_do_close(hdev);

	if (!test_bit(HCI_INIT, &hdev->flags) &&
<<<<<<< HEAD
				!test_bit(HCI_SETUP, &hdev->flags) &&
				hdev->dev_type == HCI_BREDR) {
		hci_dev_lock_bh(hdev);
		mgmt_index_removed(hdev->id);
		hci_dev_unlock_bh(hdev);
=======
	    !hci_dev_test_flag(hdev, HCI_SETUP) &&
	    !hci_dev_test_flag(hdev, HCI_CONFIG)) {
		hci_dev_lock(hdev);
		mgmt_index_removed(hdev);
		hci_dev_unlock(hdev);
>>>>>>> android-4.9
	}

	if (!IS_ERR(hdev->tfm))
		crypto_free_blkcipher(hdev->tfm);

	hci_sock_dev_event(hdev, HCI_DEV_UNREG);

	if (hdev->rfkill) {
		rfkill_unregister(hdev->rfkill);
		rfkill_destroy(hdev->rfkill);
	}

<<<<<<< HEAD
	hci_unregister_sysfs(hdev);

	/* Disable all timers */
	hci_del_off_timer(hdev);
	del_timer(&hdev->adv_timer);
	del_timer(&hdev->cmd_timer);
	del_timer(&hdev->disco_timer);
	del_timer(&hdev->disco_le_timer);
=======
	device_del(&hdev->dev);

	debugfs_remove_recursive(hdev->debugfs);
	kfree_const(hdev->hw_info);
	kfree_const(hdev->fw_info);
>>>>>>> android-4.9

	destroy_workqueue(hdev->workqueue);
	destroy_workqueue(hdev->req_workqueue);

<<<<<<< HEAD
	hci_dev_lock_bh(hdev);
	hci_blacklist_clear(hdev);
	hci_uuids_clear(hdev);
	hci_link_keys_clear(hdev);
	hci_remote_oob_data_clear(hdev);
	hci_adv_entries_clear(hdev);
	hci_dev_unlock_bh(hdev);

	__hci_dev_put(hdev);

	return 0;
=======
	hci_dev_lock(hdev);
	hci_bdaddr_list_clear(&hdev->blacklist);
	hci_bdaddr_list_clear(&hdev->whitelist);
	hci_uuids_clear(hdev);
	hci_link_keys_clear(hdev);
	hci_smp_ltks_clear(hdev);
	hci_smp_irks_clear(hdev);
	hci_remote_oob_data_clear(hdev);
	hci_adv_instances_clear(hdev);
	hci_bdaddr_list_clear(&hdev->le_white_list);
	hci_conn_params_clear_all(hdev);
	hci_discovery_filter_clear(hdev);
	hci_dev_unlock(hdev);

	hci_dev_put(hdev);

	ida_simple_remove(&hci_index_ida, id);
>>>>>>> android-4.9
}
EXPORT_SYMBOL(hci_unregister_dev);

/* Suspend HCI device */
int hci_suspend_dev(struct hci_dev *hdev)
{
	hci_sock_dev_event(hdev, HCI_DEV_SUSPEND);
	return 0;
}
EXPORT_SYMBOL(hci_suspend_dev);

/* Resume HCI device */
int hci_resume_dev(struct hci_dev *hdev)
{
	hci_sock_dev_event(hdev, HCI_DEV_RESUME);
	return 0;
}
EXPORT_SYMBOL(hci_resume_dev);

/* Reset HCI device */
int hci_reset_dev(struct hci_dev *hdev)
{
	const u8 hw_err[] = { HCI_EV_HARDWARE_ERROR, 0x01, 0x00 };
	struct sk_buff *skb;

	skb = bt_skb_alloc(3, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	hci_skb_pkt_type(skb) = HCI_EVENT_PKT;
	memcpy(skb_put(skb, 3), hw_err, 3);

	/* Send Hardware Error to upper stack */
	return hci_recv_frame(hdev, skb);
}
EXPORT_SYMBOL(hci_reset_dev);

/* Receive frame from HCI drivers */
int hci_recv_frame(struct hci_dev *hdev, struct sk_buff *skb)
{
	if (!hdev || (!test_bit(HCI_UP, &hdev->flags)
		      && !test_bit(HCI_INIT, &hdev->flags))) {
		kfree_skb(skb);
		return -ENXIO;
	}

	if (hci_skb_pkt_type(skb) != HCI_EVENT_PKT &&
	    hci_skb_pkt_type(skb) != HCI_ACLDATA_PKT &&
	    hci_skb_pkt_type(skb) != HCI_SCODATA_PKT) {
		kfree_skb(skb);
		return -EINVAL;
	}

	/* Incoming skb */
	bt_cb(skb)->incoming = 1;

	/* Time stamp */
	__net_timestamp(skb);

	/* Queue frame for rx task */
	skb_queue_tail(&hdev->rx_q, skb);
	tasklet_schedule(&hdev->rx_task);

	return 0;
}
EXPORT_SYMBOL(hci_recv_frame);

/* Receive diagnostic message from HCI drivers */
int hci_recv_diag(struct hci_dev *hdev, struct sk_buff *skb)
{
<<<<<<< HEAD
	int len = 0;
	int hlen = 0;
	int remain = count;
	struct sk_buff *skb;
	struct bt_skb_cb *scb;

	if ((type < HCI_ACLDATA_PKT || type > HCI_EVENT_PKT) ||
				index >= NUM_REASSEMBLY)
		return -EILSEQ;

	skb = hdev->reassembly[index];

	if (!skb) {
		switch (type) {
		case HCI_ACLDATA_PKT:
			len = HCI_MAX_FRAME_SIZE;
			hlen = HCI_ACL_HDR_SIZE;
			break;
		case HCI_EVENT_PKT:
			len = HCI_MAX_EVENT_SIZE;
			hlen = HCI_EVENT_HDR_SIZE;
			break;
		case HCI_SCODATA_PKT:
			len = HCI_MAX_SCO_SIZE;
			hlen = HCI_SCO_HDR_SIZE;
			break;
		}

		skb = bt_skb_alloc(len, GFP_ATOMIC);
		if (!skb)
			return -ENOMEM;

		scb = (void *) skb->cb;
		scb->expect = hlen;
		scb->pkt_type = type;

		skb->dev = (void *) hdev;
		hdev->reassembly[index] = skb;
	}

	while (count) {
		scb = (void *) skb->cb;
		len = min(scb->expect, (__u16)count);

		memcpy(skb_put(skb, len), data, len);

		count -= len;
		data += len;
		scb->expect -= len;
		remain = count;

		switch (type) {
		case HCI_EVENT_PKT:
			if (skb->len == HCI_EVENT_HDR_SIZE) {
				struct hci_event_hdr *h = hci_event_hdr(skb);
				scb->expect = h->plen;

				if (skb_tailroom(skb) < scb->expect) {
					kfree_skb(skb);
					hdev->reassembly[index] = NULL;
					return -ENOMEM;
				}
			}
			break;

		case HCI_ACLDATA_PKT:
			if (skb->len  == HCI_ACL_HDR_SIZE) {
				struct hci_acl_hdr *h = hci_acl_hdr(skb);
				scb->expect = __le16_to_cpu(h->dlen);

				if (skb_tailroom(skb) < scb->expect) {
					kfree_skb(skb);
					hdev->reassembly[index] = NULL;
					return -ENOMEM;
				}
			}
			break;

		case HCI_SCODATA_PKT:
			if (skb->len == HCI_SCO_HDR_SIZE) {
				struct hci_sco_hdr *h = hci_sco_hdr(skb);
				scb->expect = h->dlen;

				if (skb_tailroom(skb) < scb->expect) {
					kfree_skb(skb);
					hdev->reassembly[index] = NULL;
					return -ENOMEM;
				}
			}
			break;
		}

		if (scb->expect == 0) {
			/* Complete frame */
=======
	/* Mark as diagnostic packet */
	hci_skb_pkt_type(skb) = HCI_DIAG_PKT;
>>>>>>> android-4.9

	/* Time stamp */
	__net_timestamp(skb);

	skb_queue_tail(&hdev->rx_q, skb);
	queue_work(hdev->workqueue, &hdev->rx_work);

	return 0;
}
EXPORT_SYMBOL(hci_recv_diag);

void hci_set_hw_info(struct hci_dev *hdev, const char *fmt, ...)
{
	va_list vargs;

<<<<<<< HEAD
	while (count) {
		rem = hci_reassembly(hdev, type, data, count, type - 1);
		if (rem < 0)
			return rem;

		data += (count - rem);
		count = rem;
	};

	return rem;
=======
	va_start(vargs, fmt);
	kfree_const(hdev->hw_info);
	hdev->hw_info = kvasprintf_const(GFP_KERNEL, fmt, vargs);
	va_end(vargs);
>>>>>>> android-4.9
}
EXPORT_SYMBOL(hci_set_hw_info);

void hci_set_fw_info(struct hci_dev *hdev, const char *fmt, ...)
{
	va_list vargs;

<<<<<<< HEAD
	while (count) {
		struct sk_buff *skb = hdev->reassembly[STREAM_REASSEMBLY];

		if (!skb) {
			struct { char type; } *pkt;

			/* Start of the frame */
			pkt = data;
			type = pkt->type;

			data++;
			count--;
		} else
			type = bt_cb(skb)->pkt_type;

		rem = hci_reassembly(hdev, type, data, count,
							STREAM_REASSEMBLY);
		if (rem < 0)
			return rem;

		data += (count - rem);
		count = rem;
	};

	return rem;
=======
	va_start(vargs, fmt);
	kfree_const(hdev->fw_info);
	hdev->fw_info = kvasprintf_const(GFP_KERNEL, fmt, vargs);
	va_end(vargs);
>>>>>>> android-4.9
}
EXPORT_SYMBOL(hci_set_fw_info);

/* ---- Interface to upper protocols ---- */

/* Register/Unregister protocols.
 * hci_task_lock is used to ensure that no tasks are running. */
int hci_register_proto(struct hci_proto *hp)
{
	int err = 0;

	BT_DBG("%p name %s id %d", hp, hp->name, hp->id);

	if (hp->id >= HCI_MAX_PROTO)
		return -EINVAL;

	write_lock_bh(&hci_task_lock);

	if (!hci_proto[hp->id])
		hci_proto[hp->id] = hp;
	else
		err = -EEXIST;

	write_unlock_bh(&hci_task_lock);

	return err;
}
EXPORT_SYMBOL(hci_register_proto);

int hci_unregister_proto(struct hci_proto *hp)
{
	int err = 0;

	BT_DBG("%p name %s id %d", hp, hp->name, hp->id);

	if (hp->id >= HCI_MAX_PROTO)
		return -EINVAL;

	write_lock_bh(&hci_task_lock);

	if (hci_proto[hp->id])
		hci_proto[hp->id] = NULL;
	else
		err = -ENOENT;

	write_unlock_bh(&hci_task_lock);

	return err;
}
EXPORT_SYMBOL(hci_unregister_proto);

int hci_register_cb(struct hci_cb *cb)
{
	BT_DBG("%p name %s", cb, cb->name);

<<<<<<< HEAD
	write_lock_bh(&hci_cb_list_lock);
	list_add(&cb->list, &hci_cb_list);
	write_unlock_bh(&hci_cb_list_lock);
=======
	mutex_lock(&hci_cb_list_lock);
	list_add_tail(&cb->list, &hci_cb_list);
	mutex_unlock(&hci_cb_list_lock);
>>>>>>> android-4.9

	return 0;
}
EXPORT_SYMBOL(hci_register_cb);

int hci_unregister_cb(struct hci_cb *cb)
{
	BT_DBG("%p name %s", cb, cb->name);

<<<<<<< HEAD
	write_lock_bh(&hci_cb_list_lock);
	list_del(&cb->list);
	write_unlock_bh(&hci_cb_list_lock);
=======
	mutex_lock(&hci_cb_list_lock);
	list_del(&cb->list);
	mutex_unlock(&hci_cb_list_lock);
>>>>>>> android-4.9

	return 0;
}
EXPORT_SYMBOL(hci_unregister_cb);

<<<<<<< HEAD
int hci_register_amp(struct amp_mgr_cb *cb)
{
	BT_DBG("%p", cb);

	write_lock_bh(&amp_mgr_cb_list_lock);
	list_add(&cb->list, &amp_mgr_cb_list);
	write_unlock_bh(&amp_mgr_cb_list_lock);

	return 0;
}
EXPORT_SYMBOL(hci_register_amp);

int hci_unregister_amp(struct amp_mgr_cb *cb)
{
	BT_DBG("%p", cb);

	write_lock_bh(&amp_mgr_cb_list_lock);
	list_del(&cb->list);
	write_unlock_bh(&amp_mgr_cb_list_lock);

	return 0;
}
EXPORT_SYMBOL(hci_unregister_amp);

void hci_amp_cmd_complete(struct hci_dev *hdev, __u16 opcode,
			struct sk_buff *skb)
{
	struct amp_mgr_cb *cb;

	BT_DBG("opcode 0x%x", opcode);

	read_lock_bh(&amp_mgr_cb_list_lock);
	list_for_each_entry(cb, &amp_mgr_cb_list, list) {
		if (cb->amp_cmd_complete_event)
			cb->amp_cmd_complete_event(hdev, opcode, skb);
	}
	read_unlock_bh(&amp_mgr_cb_list_lock);
}

void hci_amp_cmd_status(struct hci_dev *hdev, __u16 opcode, __u8 status)
{
	struct amp_mgr_cb *cb;

	BT_DBG("opcode 0x%x, status %d", opcode, status);

	read_lock_bh(&amp_mgr_cb_list_lock);
	list_for_each_entry(cb, &amp_mgr_cb_list, list) {
		if (cb->amp_cmd_status_event)
			cb->amp_cmd_status_event(hdev, opcode, status);
	}
	read_unlock_bh(&amp_mgr_cb_list_lock);
}

void hci_amp_event_packet(struct hci_dev *hdev, __u8 ev_code,
			struct sk_buff *skb)
{
	struct amp_mgr_cb *cb;

	BT_DBG("ev_code 0x%x", ev_code);

	read_lock_bh(&amp_mgr_cb_list_lock);
	list_for_each_entry(cb, &amp_mgr_cb_list, list) {
		if (cb->amp_event)
			cb->amp_event(hdev, ev_code, skb);
	}
	read_unlock_bh(&amp_mgr_cb_list_lock);
}

static int hci_send_frame(struct sk_buff *skb)
=======
static void hci_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
>>>>>>> android-4.9
{
	int err;

	BT_DBG("%s type %d len %d", hdev->name, hci_skb_pkt_type(skb),
	       skb->len);

	if (atomic_read(&hdev->promisc)) {
		/* Time stamp */
		__net_timestamp(skb);

		hci_send_to_sock(hdev, skb, NULL);
	}

	/* Get rid of skb owner, prior to sending to the driver. */
	skb_orphan(skb);

<<<<<<< HEAD
	hci_notify(hdev, HCI_DEV_WRITE);
	return hdev->send(skb);
=======
	if (!test_bit(HCI_RUNNING, &hdev->flags)) {
		kfree_skb(skb);
		return;
	}

	err = hdev->send(hdev, skb);
	if (err < 0) {
		BT_ERR("%s sending frame failed (%d)", hdev->name, err);
		kfree_skb(skb);
	}
>>>>>>> android-4.9
}

/* Send HCI command */
int hci_send_cmd(struct hci_dev *hdev, __u16 opcode, __u32 plen,
		 const void *param)
{
	struct sk_buff *skb;

	BT_DBG("%s opcode 0x%4.4x plen %d", hdev->name, opcode, plen);

	skb = hci_prepare_cmd(hdev, opcode, plen, param);
	if (!skb) {
		BT_ERR("%s no memory for command", hdev->name);
		return -ENOMEM;
	}

	/* Stand-alone HCI commands must be flagged as
	 * single-command requests.
	 */
	bt_cb(skb)->hci.req_flags |= HCI_REQ_START;

	skb_queue_tail(&hdev->cmd_q, skb);
	tasklet_schedule(&hdev->cmd_task);

	return 0;
}
EXPORT_SYMBOL(hci_send_cmd);

/* Get data from the previously sent command */
void *hci_sent_cmd_data(struct hci_dev *hdev, __u16 opcode)
{
	struct hci_command_hdr *hdr;

	if (!hdev->sent_cmd)
		return NULL;

	hdr = (void *) hdev->sent_cmd->data;

	if (hdr->opcode != cpu_to_le16(opcode))
		return NULL;

	BT_DBG("%s opcode 0x%4.4x", hdev->name, opcode);

	return hdev->sent_cmd->data + HCI_COMMAND_HDR_SIZE;
}

/* Send HCI command and wait for command commplete event */
struct sk_buff *hci_cmd_sync(struct hci_dev *hdev, u16 opcode, u32 plen,
			     const void *param, u32 timeout)
{
	struct sk_buff *skb;

	if (!test_bit(HCI_UP, &hdev->flags))
		return ERR_PTR(-ENETDOWN);

	bt_dev_dbg(hdev, "opcode 0x%4.4x plen %d", opcode, plen);

	hci_req_sync_lock(hdev);
	skb = __hci_cmd_sync(hdev, opcode, plen, param, timeout);
	hci_req_sync_unlock(hdev);

	return skb;
}
EXPORT_SYMBOL(hci_cmd_sync);

/* Send ACL data */
static void hci_add_acl_hdr(struct sk_buff *skb, __u16 handle, __u16 flags)
{
	struct hci_acl_hdr *hdr;
	int len = skb->len;

	skb_push(skb, HCI_ACL_HDR_SIZE);
	skb_reset_transport_header(skb);
	hdr = (struct hci_acl_hdr *)skb_transport_header(skb);
	hdr->handle = cpu_to_le16(hci_handle_pack(handle, flags));
	hdr->dlen   = cpu_to_le16(len);
}

<<<<<<< HEAD
void hci_send_acl(struct hci_conn *conn, struct hci_chan *chan,
		struct sk_buff *skb, __u16 flags)
=======
static void hci_queue_acl(struct hci_chan *chan, struct sk_buff_head *queue,
			  struct sk_buff *skb, __u16 flags)
>>>>>>> android-4.9
{
	struct hci_conn *conn = chan->conn;
	struct hci_dev *hdev = conn->hdev;
	struct sk_buff *list;

<<<<<<< HEAD
	BT_DBG("%s conn %p chan %p flags 0x%x", hdev->name, conn, chan, flags);

	skb->dev = (void *) hdev;
	bt_cb(skb)->pkt_type = HCI_ACLDATA_PKT;
	if (hdev->dev_type == HCI_BREDR)
		hci_add_acl_hdr(skb, conn->handle, flags);
	else
		hci_add_acl_hdr(skb, chan->ll_handle, flags);
=======
	skb->len = skb_headlen(skb);
	skb->data_len = 0;

	hci_skb_pkt_type(skb) = HCI_ACLDATA_PKT;

	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		hci_add_acl_hdr(skb, conn->handle, flags);
		break;
	case HCI_AMP:
		hci_add_acl_hdr(skb, chan->handle, flags);
		break;
	default:
		BT_ERR("%s unknown dev_type %d", hdev->name, hdev->dev_type);
		return;
	}
>>>>>>> android-4.9

	list = skb_shinfo(skb)->frag_list;
	if (!list) {
		/* Non fragmented */
		BT_DBG("%s nonfrag skb %p len %d", hdev->name, skb, skb->len);

		skb_queue_tail(&conn->data_q, skb);
	} else {
		/* Fragmented */
		BT_DBG("%s frag %p len %d", hdev->name, skb, skb->len);

		skb_shinfo(skb)->frag_list = NULL;

<<<<<<< HEAD
		/* Queue all fragments atomically */
		spin_lock_bh(&conn->data_q.lock);
=======
		/* Queue all fragments atomically. We need to use spin_lock_bh
		 * here because of 6LoWPAN links, as there this function is
		 * called from softirq and using normal spin lock could cause
		 * deadlocks.
		 */
		spin_lock_bh(&queue->lock);
>>>>>>> android-4.9

		__skb_queue_tail(&conn->data_q, skb);
		flags &= ~ACL_PB_MASK;
		flags |= ACL_CONT;
		do {
			skb = list; list = list->next;

			hci_skb_pkt_type(skb) = HCI_ACLDATA_PKT;
			hci_add_acl_hdr(skb, conn->handle, flags);

			BT_DBG("%s frag %p len %d", hdev->name, skb, skb->len);

			__skb_queue_tail(&conn->data_q, skb);
		} while (list);

<<<<<<< HEAD
		spin_unlock_bh(&conn->data_q.lock);
=======
		spin_unlock_bh(&queue->lock);
>>>>>>> android-4.9
	}

<<<<<<< HEAD
	tasklet_schedule(&hdev->tx_task);
=======
void hci_send_acl(struct hci_chan *chan, struct sk_buff *skb, __u16 flags)
{
	struct hci_dev *hdev = chan->conn->hdev;

	BT_DBG("%s chan %p flags 0x%4.4x", hdev->name, chan, flags);

	hci_queue_acl(chan, &chan->data_q, skb, flags);

	queue_work(hdev->workqueue, &hdev->tx_work);
>>>>>>> android-4.9
}

/* Send SCO data */
void hci_send_sco(struct hci_conn *conn, struct sk_buff *skb)
{
	struct hci_dev *hdev = conn->hdev;
	struct hci_sco_hdr hdr;

	BT_DBG("%s len %d", hdev->name, skb->len);

	hdr.handle = cpu_to_le16(conn->handle);
	hdr.dlen   = skb->len;

	skb_push(skb, HCI_SCO_HDR_SIZE);
	skb_reset_transport_header(skb);
	memcpy(skb_transport_header(skb), &hdr, HCI_SCO_HDR_SIZE);

	hci_skb_pkt_type(skb) = HCI_SCODATA_PKT;

	skb_queue_tail(&conn->data_q, skb);
	tasklet_schedule(&hdev->tx_task);
}

/* ---- HCI TX task (outgoing data) ---- */
/* HCI ACL Connection scheduler */
static inline struct hci_conn *hci_low_sent_acl(struct hci_dev *hdev,
								int *quote)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn *conn = NULL;
	int num = 0, min = ~0, conn_num = 0;
	struct list_head *p;

	/* We don't have to lock device here. Connections are always
	 * added and removed with TX task disabled. */
	list_for_each(p, &h->list) {
		struct hci_conn *c;
		c = list_entry(p, struct hci_conn, list);
		if (c->type == ACL_LINK)
			conn_num++;

		if (skb_queue_empty(&c->data_q))
			continue;

		if (c->state != BT_CONNECTED && c->state != BT_CONFIG)
			continue;

		num++;

		if (c->sent < min) {
			min  = c->sent;
			conn = c;
		}
	}

	if (conn) {
		int cnt, q;
		cnt = hdev->acl_cnt;
		q = cnt / num;
		*quote = q ? q : 1;
	} else
		*quote = 0;

	if ((*quote == hdev->acl_cnt) &&
		(conn->sent == (hdev->acl_pkts - 1)) &&
		(conn_num > 1)) {
			*quote = 0;
			conn = NULL;
	}

	BT_DBG("conn %p quote %d", conn, *quote);
	return conn;
}

/* HCI Connection scheduler */
static struct hci_conn *hci_low_sent(struct hci_dev *hdev, __u8 type,
				     int *quote)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
<<<<<<< HEAD
	struct hci_conn *conn = NULL;
	int num = 0, min = ~0;
	struct list_head *p;
=======
	struct hci_conn *conn = NULL, *c;
	unsigned int num = 0, min = ~0;
>>>>>>> android-4.9

	/* We don't have to lock device here. Connections are always
	 * added and removed with TX task disabled. */
	list_for_each(p, &h->list) {
		struct hci_conn *c;
		c = list_entry(p, struct hci_conn, list);

		if (c->type != type || skb_queue_empty(&c->data_q))
			continue;

		if (c->state != BT_CONNECTED && c->state != BT_CONFIG)
			continue;

		num++;

		if (c->sent < min) {
			min  = c->sent;
			conn = c;
		}
	}

	if (conn) {
		int cnt, q;

		switch (conn->type) {
		case ACL_LINK:
			cnt = hdev->acl_cnt;
			break;
		case SCO_LINK:
		case ESCO_LINK:
			cnt = hdev->sco_cnt;
			break;
		case LE_LINK:
			cnt = hdev->le_mtu ? hdev->le_cnt : hdev->acl_cnt;
			break;
		default:
			cnt = 0;
			BT_ERR("Unknown link type");
		}

		q = cnt / num;
		*quote = q ? q : 1;
	} else
		*quote = 0;

	BT_DBG("conn %p quote %d", conn, *quote);
	return conn;
}

static void hci_link_tx_to(struct hci_dev *hdev, __u8 type)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct list_head *p;
	struct hci_conn  *c;

	BT_ERR("%s link tx timeout", hdev->name);

	/* Kill stalled connections */
	list_for_each(p, &h->list) {
		c = list_entry(p, struct hci_conn, list);
		if (c->type == type && c->sent) {
			BT_ERR("%s killing stalled connection %pMR",
			       hdev->name, &c->dst);
			hci_disconnect(c, HCI_ERROR_REMOTE_USER_TERM);
		}
	}
}

<<<<<<< HEAD
static inline void hci_sched_acl(struct hci_dev *hdev)
=======
static struct hci_chan *hci_chan_sent(struct hci_dev *hdev, __u8 type,
				      int *quote)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_chan *chan = NULL;
	unsigned int num = 0, min = ~0, cur_prio = 0;
	struct hci_conn *conn;
	int cnt, q, conn_num = 0;

	BT_DBG("%s", hdev->name);

	rcu_read_lock();

	list_for_each_entry_rcu(conn, &h->list, list) {
		struct hci_chan *tmp;

		if (conn->type != type)
			continue;

		if (conn->state != BT_CONNECTED && conn->state != BT_CONFIG)
			continue;

		conn_num++;

		list_for_each_entry_rcu(tmp, &conn->chan_list, list) {
			struct sk_buff *skb;

			if (skb_queue_empty(&tmp->data_q))
				continue;

			skb = skb_peek(&tmp->data_q);
			if (skb->priority < cur_prio)
				continue;

			if (skb->priority > cur_prio) {
				num = 0;
				min = ~0;
				cur_prio = skb->priority;
			}

			num++;

			if (conn->sent < min) {
				min  = conn->sent;
				chan = tmp;
			}
		}

		if (hci_conn_num(hdev, type) == conn_num)
			break;
	}

	rcu_read_unlock();

	if (!chan)
		return NULL;

	switch (chan->conn->type) {
	case ACL_LINK:
		cnt = hdev->acl_cnt;
		break;
	case AMP_LINK:
		cnt = hdev->block_cnt;
		break;
	case SCO_LINK:
	case ESCO_LINK:
		cnt = hdev->sco_cnt;
		break;
	case LE_LINK:
		cnt = hdev->le_mtu ? hdev->le_cnt : hdev->acl_cnt;
		break;
	default:
		cnt = 0;
		BT_ERR("Unknown link type");
	}

	q = cnt / num;
	*quote = q ? q : 1;
	BT_DBG("chan %p quote %d", chan, *quote);
	return chan;
}

static void hci_prio_recalculate(struct hci_dev *hdev, __u8 type)
>>>>>>> android-4.9
{
	struct hci_conn *conn;
	struct sk_buff *skb;
	int quote;

	BT_DBG("%s", hdev->name);

<<<<<<< HEAD
	if (!test_bit(HCI_RAW, &hdev->flags)) {
		/* ACL tx timeout must be longer than maximum
		 * link supervision timeout (40.9 seconds) */
		if (hdev->acl_cnt <= 0 &&
			time_after(jiffies, hdev->acl_last_tx + HZ * 45))
=======
	rcu_read_lock();

	list_for_each_entry_rcu(conn, &h->list, list) {
		struct hci_chan *chan;

		if (conn->type != type)
			continue;

		if (conn->state != BT_CONNECTED && conn->state != BT_CONFIG)
			continue;

		num++;

		list_for_each_entry_rcu(chan, &conn->chan_list, list) {
			struct sk_buff *skb;

			if (chan->sent) {
				chan->sent = 0;
				continue;
			}

			if (skb_queue_empty(&chan->data_q))
				continue;

			skb = skb_peek(&chan->data_q);
			if (skb->priority >= HCI_PRIO_MAX - 1)
				continue;

			skb->priority = HCI_PRIO_MAX - 1;

			BT_DBG("chan %p skb %p promoted to %d", chan, skb,
			       skb->priority);
		}

		if (hci_conn_num(hdev, type) == num)
			break;
	}

	rcu_read_unlock();

}

static inline int __get_blocks(struct hci_dev *hdev, struct sk_buff *skb)
{
	/* Calculate count of blocks used by this packet */
	return DIV_ROUND_UP(skb->len - HCI_ACL_HDR_SIZE, hdev->block_len);
}

static void __check_timeout(struct hci_dev *hdev, unsigned int cnt)
{
	if (!hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
		/* ACL tx timeout must be longer than maximum
		 * link supervision timeout (40.9 seconds) */
		if (!cnt && time_after(jiffies, hdev->acl_last_tx +
				       HCI_ACL_TX_TIMEOUT))
>>>>>>> android-4.9
			hci_link_tx_to(hdev, ACL_LINK);
	}

<<<<<<< HEAD
	while (hdev->acl_cnt > 0 &&
		((conn = hci_low_sent_acl(hdev, &quote)) != NULL)) {

		while (quote > 0 &&
			  (skb = skb_dequeue(&conn->data_q))) {
			int count = 1;

			BT_DBG("skb %p len %d", skb, skb->len);
=======
static void hci_sched_acl_pkt(struct hci_dev *hdev)
{
	unsigned int cnt = hdev->acl_cnt;
	struct hci_chan *chan;
	struct sk_buff *skb;
	int quote;

	__check_timeout(hdev, cnt);

	while (hdev->acl_cnt &&
	       (chan = hci_chan_sent(hdev, ACL_LINK, &quote))) {
		u32 priority = (skb_peek(&chan->data_q))->priority;
		while (quote-- && (skb = skb_peek(&chan->data_q))) {
			BT_DBG("chan %p skb %p len %d priority %u", chan, skb,
			       skb->len, skb->priority);

			/* Stop if priority has changed */
			if (skb->priority < priority)
				break;

			skb = skb_dequeue(&chan->data_q);

			hci_conn_enter_active_mode(chan->conn,
						   bt_cb(skb)->force_active);

			hci_send_frame(hdev, skb);
			hdev->acl_last_tx = jiffies;

			hdev->acl_cnt--;
			chan->sent++;
			chan->conn->sent++;
		}
	}

	if (cnt != hdev->acl_cnt)
		hci_prio_recalculate(hdev, ACL_LINK);
}

static void hci_sched_acl_blk(struct hci_dev *hdev)
{
	unsigned int cnt = hdev->block_cnt;
	struct hci_chan *chan;
	struct sk_buff *skb;
	int quote;
	u8 type;

	__check_timeout(hdev, cnt);

	BT_DBG("%s", hdev->name);

	if (hdev->dev_type == HCI_AMP)
		type = AMP_LINK;
	else
		type = ACL_LINK;

	while (hdev->block_cnt > 0 &&
	       (chan = hci_chan_sent(hdev, type, &quote))) {
		u32 priority = (skb_peek(&chan->data_q))->priority;
		while (quote > 0 && (skb = skb_peek(&chan->data_q))) {
			int blocks;

			BT_DBG("chan %p skb %p len %d priority %u", chan, skb,
			       skb->len, skb->priority);

			/* Stop if priority has changed */
			if (skb->priority < priority)
				break;
>>>>>>> android-4.9

			if (hdev->flow_ctl_mode ==
				HCI_BLOCK_BASED_FLOW_CTL_MODE)
				/* Calculate count of blocks used by
				 * this packet
				 */
				count = ((skb->len - HCI_ACL_HDR_SIZE - 1) /
					hdev->data_block_len) + 1;

			if (count > hdev->acl_cnt)
				return;

<<<<<<< HEAD
			hci_conn_enter_active_mode(conn, bt_cb(skb)->force_active);
=======
			hci_conn_enter_active_mode(chan->conn,
						   bt_cb(skb)->force_active);
>>>>>>> android-4.9

			hci_send_frame(hdev, skb);
			hdev->acl_last_tx = jiffies;

			hdev->acl_cnt -= count;
			quote -= count;

			conn->sent += count;
		}
	}
<<<<<<< HEAD
=======

	if (cnt != hdev->block_cnt)
		hci_prio_recalculate(hdev, type);
}

static void hci_sched_acl(struct hci_dev *hdev)
{
	BT_DBG("%s", hdev->name);

	/* No ACL link over BR/EDR controller */
	if (!hci_conn_num(hdev, ACL_LINK) && hdev->dev_type == HCI_PRIMARY)
		return;

	/* No AMP link over AMP controller */
	if (!hci_conn_num(hdev, AMP_LINK) && hdev->dev_type == HCI_AMP)
		return;

	switch (hdev->flow_ctl_mode) {
	case HCI_FLOW_CTL_MODE_PACKET_BASED:
		hci_sched_acl_pkt(hdev);
		break;

	case HCI_FLOW_CTL_MODE_BLOCK_BASED:
		hci_sched_acl_blk(hdev);
		break;
	}
>>>>>>> android-4.9
}

/* Schedule SCO */
static void hci_sched_sco(struct hci_dev *hdev)
{
	struct hci_conn *conn;
	struct sk_buff *skb;
	int quote;

	BT_DBG("%s", hdev->name);

	while (hdev->sco_cnt && (conn = hci_low_sent(hdev, SCO_LINK, &quote))) {
		while (quote-- && (skb = skb_dequeue(&conn->data_q))) {
			BT_DBG("skb %p len %d", skb, skb->len);
			hci_send_frame(hdev, skb);

			conn->sent++;
			if (conn->sent == ~0)
				conn->sent = 0;
		}
	}
}

static void hci_sched_esco(struct hci_dev *hdev)
{
	struct hci_conn *conn;
	struct sk_buff *skb;
	int quote;

	BT_DBG("%s", hdev->name);

<<<<<<< HEAD
	while (hdev->sco_cnt && (conn = hci_low_sent(hdev, ESCO_LINK, &quote))) {
=======
	if (!hci_conn_num(hdev, ESCO_LINK))
		return;

	while (hdev->sco_cnt && (conn = hci_low_sent(hdev, ESCO_LINK,
						     &quote))) {
>>>>>>> android-4.9
		while (quote-- && (skb = skb_dequeue(&conn->data_q))) {
			BT_DBG("skb %p len %d", skb, skb->len);
			hci_send_frame(hdev, skb);

			conn->sent++;
			if (conn->sent == ~0)
				conn->sent = 0;
		}
	}
}

static void hci_sched_le(struct hci_dev *hdev)
{
	struct hci_conn *conn;
	struct sk_buff *skb;
	int quote, cnt;

	BT_DBG("%s", hdev->name);

<<<<<<< HEAD
	if (!test_bit(HCI_RAW, &hdev->flags)) {
=======
	if (!hci_conn_num(hdev, LE_LINK))
		return;

	if (!hci_dev_test_flag(hdev, HCI_UNCONFIGURED)) {
>>>>>>> android-4.9
		/* LE tx timeout must be longer than maximum
		 * link supervision timeout (40.9 seconds) */
		if (!hdev->le_cnt && hdev->le_pkts &&
		    time_after(jiffies, hdev->le_last_tx + HZ * 45))
			hci_link_tx_to(hdev, LE_LINK);
	}

	cnt = hdev->le_pkts ? hdev->le_cnt : hdev->acl_cnt;
<<<<<<< HEAD
	while (cnt && (conn = hci_low_sent(hdev, LE_LINK, &quote))) {
		while (quote-- && (skb = skb_dequeue(&conn->data_q))) {
			BT_DBG("skb %p len %d", skb, skb->len);
=======
	tmp = cnt;
	while (cnt && (chan = hci_chan_sent(hdev, LE_LINK, &quote))) {
		u32 priority = (skb_peek(&chan->data_q))->priority;
		while (quote-- && (skb = skb_peek(&chan->data_q))) {
			BT_DBG("chan %p skb %p len %d priority %u", chan, skb,
			       skb->len, skb->priority);

			/* Stop if priority has changed */
			if (skb->priority < priority)
				break;

			skb = skb_dequeue(&chan->data_q);
>>>>>>> android-4.9

			hci_send_frame(hdev, skb);
			hdev->le_last_tx = jiffies;

			cnt--;
			conn->sent++;
		}
	}
	if (hdev->le_pkts)
		hdev->le_cnt = cnt;
	else
		hdev->acl_cnt = cnt;
}

static void hci_tx_task(unsigned long arg)
{
	struct hci_dev *hdev = (struct hci_dev *) arg;
	struct sk_buff *skb;

	read_lock(&hci_task_lock);

	BT_DBG("%s acl %d sco %d le %d", hdev->name, hdev->acl_cnt,
	       hdev->sco_cnt, hdev->le_cnt);

	if (!hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
		/* Schedule queues and send stuff to HCI driver */
		hci_sched_acl(hdev);
		hci_sched_sco(hdev);
		hci_sched_esco(hdev);
		hci_sched_le(hdev);
	}

	/* Send next queued raw (unknown type) packet */
	while ((skb = skb_dequeue(&hdev->raw_q)))
<<<<<<< HEAD
		hci_send_frame(skb);

	read_unlock(&hci_task_lock);
=======
		hci_send_frame(hdev, skb);
>>>>>>> android-4.9
}

/* ----- HCI RX task (incoming data proccessing) ----- */

/* ACL data packet */
static void hci_acldata_packet(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_acl_hdr *hdr = (void *) skb->data;
	struct hci_conn *conn;
	__u16 handle, flags;

	skb_pull(skb, HCI_ACL_HDR_SIZE);

	handle = __le16_to_cpu(hdr->handle);
	flags  = hci_flags(handle);
	handle = hci_handle(handle);

	BT_DBG("%s len %d handle 0x%4.4x flags 0x%4.4x", hdev->name, skb->len,
	       handle, flags);

	hdev->stat.acl_rx++;

	hci_dev_lock(hdev);
	conn = hci_conn_hash_lookup_handle(hdev, handle);
	hci_dev_unlock(hdev);

	if (conn) {
		register struct hci_proto *hp;

<<<<<<< HEAD
		hci_conn_enter_active_mode(conn, bt_cb(skb)->force_active);

=======
>>>>>>> android-4.9
		/* Send to upper protocol */
		hp = hci_proto[HCI_PROTO_L2CAP];
		if (hp && hp->recv_acldata) {
			hp->recv_acldata(conn, skb, flags);
			return;
		}
	} else {
		BT_ERR("%s ACL packet for unknown connection handle %d",
		       hdev->name, handle);
	}

	kfree_skb(skb);
}

/* SCO data packet */
static void hci_scodata_packet(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_sco_hdr *hdr = (void *) skb->data;
	struct hci_conn *conn;
	__u16 handle;

	skb_pull(skb, HCI_SCO_HDR_SIZE);

	handle = __le16_to_cpu(hdr->handle);

	BT_DBG("%s len %d handle 0x%4.4x", hdev->name, skb->len, handle);

	hdev->stat.sco_rx++;

	hci_dev_lock(hdev);
	conn = hci_conn_hash_lookup_handle(hdev, handle);
	hci_dev_unlock(hdev);

	if (conn) {
		register struct hci_proto *hp;

		/* Send to upper protocol */
		hp = hci_proto[HCI_PROTO_SCO];
		if (hp && hp->recv_scodata) {
			hp->recv_scodata(conn, skb);
			return;
		}
	} else {
		BT_ERR("%s SCO packet for unknown connection handle %d",
		       hdev->name, handle);
	}

	kfree_skb(skb);
}

<<<<<<< HEAD
static void hci_rx_task(unsigned long arg)
=======
static bool hci_req_is_complete(struct hci_dev *hdev)
{
	struct sk_buff *skb;

	skb = skb_peek(&hdev->cmd_q);
	if (!skb)
		return true;

	return (bt_cb(skb)->hci.req_flags & HCI_REQ_START);
}

static void hci_resend_last(struct hci_dev *hdev)
{
	struct hci_command_hdr *sent;
	struct sk_buff *skb;
	u16 opcode;

	if (!hdev->sent_cmd)
		return;

	sent = (void *) hdev->sent_cmd->data;
	opcode = __le16_to_cpu(sent->opcode);
	if (opcode == HCI_OP_RESET)
		return;

	skb = skb_clone(hdev->sent_cmd, GFP_KERNEL);
	if (!skb)
		return;

	skb_queue_head(&hdev->cmd_q, skb);
	queue_work(hdev->workqueue, &hdev->cmd_work);
}

void hci_req_cmd_complete(struct hci_dev *hdev, u16 opcode, u8 status,
			  hci_req_complete_t *req_complete,
			  hci_req_complete_skb_t *req_complete_skb)
{
	struct sk_buff *skb;
	unsigned long flags;

	BT_DBG("opcode 0x%04x status 0x%02x", opcode, status);

	/* If the completed command doesn't match the last one that was
	 * sent we need to do special handling of it.
	 */
	if (!hci_sent_cmd_data(hdev, opcode)) {
		/* Some CSR based controllers generate a spontaneous
		 * reset complete event during init and any pending
		 * command will never be completed. In such a case we
		 * need to resend whatever was the last sent
		 * command.
		 */
		if (test_bit(HCI_INIT, &hdev->flags) && opcode == HCI_OP_RESET)
			hci_resend_last(hdev);

		return;
	}

	/* If the command succeeded and there's still more commands in
	 * this request the request is not yet complete.
	 */
	if (!status && !hci_req_is_complete(hdev))
		return;

	/* If this was the last command in a request the complete
	 * callback would be found in hdev->sent_cmd instead of the
	 * command queue (hdev->cmd_q).
	 */
	if (bt_cb(hdev->sent_cmd)->hci.req_flags & HCI_REQ_SKB) {
		*req_complete_skb = bt_cb(hdev->sent_cmd)->hci.req_complete_skb;
		return;
	}

	if (bt_cb(hdev->sent_cmd)->hci.req_complete) {
		*req_complete = bt_cb(hdev->sent_cmd)->hci.req_complete;
		return;
	}

	/* Remove all pending commands belonging to this request */
	spin_lock_irqsave(&hdev->cmd_q.lock, flags);
	while ((skb = __skb_dequeue(&hdev->cmd_q))) {
		if (bt_cb(skb)->hci.req_flags & HCI_REQ_START) {
			__skb_queue_head(&hdev->cmd_q, skb);
			break;
		}

		if (bt_cb(skb)->hci.req_flags & HCI_REQ_SKB)
			*req_complete_skb = bt_cb(skb)->hci.req_complete_skb;
		else
			*req_complete = bt_cb(skb)->hci.req_complete;
		kfree_skb(skb);
	}
	spin_unlock_irqrestore(&hdev->cmd_q.lock, flags);
}

static void hci_rx_work(struct work_struct *work)
>>>>>>> android-4.9
{
	struct hci_dev *hdev = (struct hci_dev *) arg;
	struct sk_buff *skb;

	BT_DBG("%s", hdev->name);

	read_lock(&hci_task_lock);

	while ((skb = skb_dequeue(&hdev->rx_q))) {
		if (atomic_read(&hdev->promisc)) {
			/* Send copy to the sockets */
			hci_send_to_sock(hdev, skb, NULL);
		}

		if (hci_dev_test_flag(hdev, HCI_USER_CHANNEL)) {
			kfree_skb(skb);
			continue;
		}

		if (test_bit(HCI_INIT, &hdev->flags)) {
			/* Don't process data packets in this states. */
			switch (hci_skb_pkt_type(skb)) {
			case HCI_ACLDATA_PKT:
			case HCI_SCODATA_PKT:
				kfree_skb(skb);
				continue;
			}
		}

		/* Process frame */
		switch (hci_skb_pkt_type(skb)) {
		case HCI_EVENT_PKT:
			hci_event_packet(hdev, skb);
			break;

		case HCI_ACLDATA_PKT:
			BT_DBG("%s ACL data packet", hdev->name);
			hci_acldata_packet(hdev, skb);
			break;

		case HCI_SCODATA_PKT:
			BT_DBG("%s SCO data packet", hdev->name);
			hci_scodata_packet(hdev, skb);
			break;

		default:
			kfree_skb(skb);
			break;
		}
	}

	read_unlock(&hci_task_lock);
}

static void hci_cmd_task(unsigned long arg)
{
	struct hci_dev *hdev = (struct hci_dev *) arg;
	struct sk_buff *skb;

	BT_DBG("%s cmd_cnt %d cmd queued %d", hdev->name,
	       atomic_read(&hdev->cmd_cnt), skb_queue_len(&hdev->cmd_q));

	/* Send queued commands */
	if (atomic_read(&hdev->cmd_cnt)) {
		skb = skb_dequeue(&hdev->cmd_q);
		if (!skb)
			return;

		kfree_skb(hdev->sent_cmd);

		hdev->sent_cmd = skb_clone(skb, GFP_KERNEL);
		if (hdev->sent_cmd) {
			atomic_dec(&hdev->cmd_cnt);
<<<<<<< HEAD
			hci_send_frame(skb);
			mod_timer(&hdev->cmd_timer,
				  jiffies + msecs_to_jiffies(HCI_CMD_TIMEOUT));
=======
			hci_send_frame(hdev, skb);
			if (test_bit(HCI_RESET, &hdev->flags))
				cancel_delayed_work(&hdev->cmd_timer);
			else
				schedule_delayed_work(&hdev->cmd_timer,
						      HCI_CMD_TIMEOUT);
>>>>>>> android-4.9
		} else {
			skb_queue_head(&hdev->cmd_q, skb);
			tasklet_schedule(&hdev->cmd_task);
		}
	}
}
<<<<<<< HEAD

module_param(enable_smp, bool, 0644);
MODULE_PARM_DESC(enable_smp, "Enable SMP support (LE only)");
=======
>>>>>>> android-4.9
