#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

#define USB_VENDOR_ID 0xc0f4
#define USB_PRODUCT_ID 0x01e0

void print_interface_descriptor(struct usb_interface_descriptor i)
{
    pr_info("USB_INTERFACE_DESCRIPTOR:\n");
    pr_info("bLength: 0x%x\n", i.bLength);
    pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);
    pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);
    pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);
    pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);
    pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);
    pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);
    pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);
    pr_info("iInterface: 0x%x\n", i.iInterface);
    pr_info("\n");
}

void print_usb_endpoint_descriptor(struct usb_endpoint_descriptor e)
{
    pr_info("USB_ENDPOINT_DESCRIPTOR:\n");
    pr_info("bLength: 0x%x\n", e.bLength);
    pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);
    pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);
    pr_info("bmAttributes: 0x%x\n", e.bmAttributes);
    pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);
    pr_info("bInterval: 0x%x\n", e.bInterval);
    pr_info("\n");
}

static int k_usb_probe(struct usb_interface *interface,
                       const struct usb_device_id *id)
{
    unsigned int i;
    unsigned int ep_count;
    struct usb_host_interface *iface_desc = interface->cur_altsetting;

    dev_info(&interface->dev, "Vendor ID : 0x%02x, Product ID : 0x%02x\n", id->idVendor, id->idProduct);

    ep_count = iface_desc->desc.bNumEndpoints;

    print_interface_descriptor(iface_desc->desc);

    for (i = 0; i < ep_count; i++) {
        print_usb_endpoint_descriptor(iface_desc->endpoint[i].desc);
    }

    return 0;
}

static void k_usb_disconnect(struct usb_interface *interface)
{
    dev_info(&interface->dev, "USB Driver Disconnected\n");
}

const struct usb_device_id k_usb_table[] = {
    {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
    {},
};

MODULE_DEVICE_TABLE(usb, k_usb_table);

static struct usb_driver k_usb_driver = {
    .name = "K USB Driver",
    .probe = k_usb_probe,
    .disconnect = k_usb_disconnect,
    .id_table = k_usb_table,
};

module_usb_driver(k_usb_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("K <k@k.com>");
MODULE_DESCRIPTION("k usb");
MODULE_VERSION("1.0.0");
