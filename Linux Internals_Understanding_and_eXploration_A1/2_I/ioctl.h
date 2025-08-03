#ifndef __IOCTL_DEFINE_H__
#define __IOCTL_DEFINE_H__

#define IOCTL_BASE	'W'
// used in the main ioctl program inside switch case. THis is the identifier of the switch case and here I have initilized them.
#define IOCTL_VIR_TO_PHY _IOW(IOCTL_BASE, 3, struct ioctl_data)
#define IOCTL_WRITE_TO_PHY _IOW(IOCTL_BASE, 2, struct write_data)


#endif // __IOCTL_DEFINE_H__