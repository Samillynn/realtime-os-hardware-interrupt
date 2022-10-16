/**
 * enable the group enable bit in both distributor and CPU interface for each INTID
 * enable specific INTID
 * SPI -> gicd_irouter[]
 * PPI <- which are PPI, which int type does System Clock belongs to? can check using gicd->type?
 * SGI -> customized by software
 * GICC priority mask
 * 
 * Take an interrupt:
 *  Gic iar0 & iar1
 * 
*/

/** GIC type is 11111 1 00 011 00111 **/

#ifndef __GIC_V2__
#define __GIC_V2__

#include "common.h"

STRUCT(GICD) {
  u32 ctl;
  u32 type;
  u32 iid;
  u32 _unused1[29];

  u32 igroup;
  u32 _unused2[31];

  u32 isenable[32];
  u32 icenable[32];
  u32 ispend[32];
  u32 icpend[32];
  u32 isactive[32];
  u32 icactive[32];

  u32 ipriority[255];
  u32 _unused3;

  u32 itargets[255];
  u32 _unused4;

  u32 icfg[64];
  u32 _unused5[64];
  u32 nsac[64];

  u32 sgi;
  u32 _unused6[3];
  u32 cpendsgi[4];
  u32 spendsgi[4];
  u32 _unused7[52];
};

STRUCT(GICC) {
  u32 ctl;
  u32 pm;
  u32 bp;
  u32 ia;
  u32 eoi;
  u32 rp;
  u32 hppi;
  u32 abp;
  u32 aia;
  u32 aeoi;
  u32 ahppi;
  u32 _unused1[41];
  u32 ap[4];
  u32 nsap[4];
  u32 _unused2[3];
  u32 iid;
  u32 dir;
};

extern volatile GICC* const gicc;
extern volatile GICD* const gicd;

void init_gic();
u32 ack_interrupt();

#endif
