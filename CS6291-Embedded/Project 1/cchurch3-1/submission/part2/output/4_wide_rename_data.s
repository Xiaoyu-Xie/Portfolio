 ## Target: VEX 1 cluster (big endian)
.comment ""
.comment "Copyright (C) 1990-2010 Hewlett-Packard Company"
.comment "VEX C compiler version 3.43 (20110131 release)"
.comment ""
.comment "-dir /home/vagrant/vex-3.43 -ms -fmm=myvex1.mm -fno-xnop -c"
.sversion 3.43
.rta 2
.section .bss
.align 32
.section .data
.align 32
.equ _?1TEMPLATEPACKET.9, 0x0
.equ _?1TEMPLATEPACKET.1, 0x0
 ## Begin main
.section .text
.proc
.entry caller, sp=$r0.1, rl=$l0.0, asize=-64, arg($r0.3:s32,$r0.4:u32)
main::
.trace 1
	c0    add $r0.1 = $r0.1, (-0x40)
;;								## 0
	c0    stw 0x20[$r0.1] = $l0.0  ## spill ## t85
;;								## 1
	c0    stw 0x24[$r0.1] = $r0.4  ## spill ## t99
;;								## 2
	c0    ldw $r0.3 = 4[$r0.4]   ## bblock 0, line 6, t3, t99
;;								## 3
;;								## 4
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 0, line 6,  raddr(atoi)(P32),  t3
;;								## 5
	c0    ldw $r0.4 = 0x24[$r0.1]  ## restore ## t99
;;								## 6
	c0    stw 0x10[$r0.1] = $r0.3   ## bblock 1, line 6, t86, t0
;;								## 7
;;								## 8
	c0    ldw $r0.3 = 8[$r0.4]   ## bblock 1, line 7, t7, t99
;;								## 9
;;								## 10
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 1, line 7,  raddr(atoi)(P32),  t7
;;								## 11
	c0    stw 0x28[$r0.1] = $r0.3  ## spill ## t4
;;								## 12
	c0    ldw $r0.4 = 0x24[$r0.1]  ## restore ## t99
;;								## 13
;;								## 14
;;								## 15
	c0    ldw $r0.3 = 12[$r0.4]   ## bblock 2, line 8, t11, t99
;;								## 16
;;								## 17
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 2, line 8,  raddr(atoi)(P32),  t11
;;								## 18
	c0    ldw $r0.4 = 0x24[$r0.1]  ## restore ## t99
;;								## 19
	c0    stw 0x14[$r0.1] = $r0.3   ## bblock 3, line 8, t86, t8
;;								## 20
;;								## 21
	c0    ldw $r0.3 = 16[$r0.4]   ## bblock 3, line 9, t15, t99
;;								## 22
;;								## 23
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 3, line 9,  raddr(atoi)(P32),  t15
;;								## 24
	c0    ldw $r0.4 = 0x24[$r0.1]  ## restore ## t99
;;								## 25
;;								## 26
;;								## 27
	c0    ldw $r0.3 = 20[$r0.4]   ## bblock 4, line 10, t19, t99
;;								## 28
;;								## 29
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 4, line 10,  raddr(atoi)(P32),  t19
;;								## 30
	c0    ldw $r0.4 = 0x24[$r0.1]  ## restore ## t99
;;								## 31
	c0    stw 0x18[$r0.1] = $r0.3   ## bblock 5, line 10, t86, t16
;;								## 32
;;								## 33
	c0    ldw $r0.3 = 24[$r0.4]   ## bblock 5, line 11, t23, t99
;;								## 34
;;								## 35
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi 
;;
	c0    ldw $r0.14 = 0x18[$r0.1] 
	c0    add $r0.5 = $r0.1, 0x18 
	c0    add $r0.6 = $r0.1, 0x1c 
	c0    stw 0x1c[$r0.1] = $r0.3 
;;
	c0    add $r0.7 = $r0.1, 0x14 
	c0    add $r0.9 = $r0.1, 0x10 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.2 = 0x10[$r0.1] 
	c0    add $r0.8 = $r0.3, $r0.14 
;;
	c0    mov $r0.3 = (_?1STRINGPACKET.1 + 0) 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.15 = 0x14[$r0.1] 
	c0    cmpgt $b0.0 = $r0.2, 10 
;;
	c0    slct $r0.13 = $b0.0, $r0.5, $r0.6 
	c0    slct $r0.17 = $b0.0, $r0.7, $r0.9 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.16 = $r0.15, 5 
	c0    stw 0x14[$r0.1] = $r0.8 
	c0    ldw $r0.20 = 0x28[$r0.1] 
;;
	c0    stw 0x10[$r0.1] = $r0.16 
	c0    add $r0.18 = $r0.14, $r0.16 
;;
	c0    add $r0.10 = $r0.18, -20 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.19 = 0[$r0.13] 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.25 = $r0.19, $r0.18 
;;
	c0    stw 0[$r0.17] = $r0.25 
;;
	c0    ldw $r0.27 = 0[$r0.13] 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.24 = 0x10[$r0.1] 
	c0    add $r0.21 = $r0.20, $r0.27 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.29 = $r0.21, $r0.24 
;;
	c0    stw 0x14[$r0.1] = $r0.29 
;;
	c0    ldw $r0.30 = 0[$r0.17] 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.32 = $r0.29, $r0.30 
;;
	c0    stw 0[$r0.13] = $r0.32 
;;
	c0    ldw $r0.33 = 0x18[$r0.1] 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.34 = $r0.32, $r0.33 
	c0    ldw $r0.31 = 0x14[$r0.1] 
;;
	c0    stw 0x1c[$r0.1] = $r0.34 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.38 = 0[$r0.17] 
	c0    add $r0.22 = $r0.10, $r0.31 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.12 = 0[$r0.13] 
	c0    add $r0.36 = $r0.38, $r0.31 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.41 = $r0.36, $r0.12 
	c0    ldw $r0.11 = 0x10[$r0.1] 
;;
	c0    stw 0x18[$r0.1] = $r0.41 
	c0    add $r0.42 = $r0.38, $r0.41 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.28 = 0[$r0.17] 
	c0    add $r0.23 = $r0.18, $r0.11 
;;
	c0    add $r0.39 = $r0.23, $r0.42 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    ldw $r0.26 = 0[$r0.13] 
	c0    add $r0.35 = $r0.33, $r0.28 
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
;;
	c0    add $r0.40 = $r0.34, $r0.26 
;;
	c0    add $r0.43 = $r0.35, $r0.40 
;;
	c0    add $r0.37 = $r0.22, $r0.43 
;;
	c0    add $r0.4 = $r0.37, $r0.39 
;;
.call printf, caller, arg($r0.3:u32,$r0.4:s32), ret($r0.3:s32)
	c0    call $l0.0 = printf   ## bblock 6, line 33,  raddr(printf)(P32),  addr(_?1STRINGVAR.1)(P32),  t84
;;								## 95
	c0    ldw $l0.0 = 0x20[$r0.1]  ## restore ## t85
;;								## 96
;;								## 97
;;								## 98
;;								## 99
.return ret()
	c0    return $r0.1 = $r0.1, (0x40), $l0.0   ## bblock 9, line 34,  t86,  ?2.1?2auto_size(I32),  t85
;;								## 100
.endp
.section .bss
.section .data
_?1STRINGPACKET.1:
    .data1 37
    .data1 100
    .data1 10
    .data1 0
.equ ?2.1?2scratch.0, 0x0
.equ _?1PACKET.1, 0x10
.equ _?1PACKET.3, 0x14
.equ _?1PACKET.5, 0x18
.equ _?1PACKET.6, 0x1c
.equ ?2.1?2ras_p, 0x20
.equ ?2.1?2spill_p, 0x24
.section .data
.section .text
.equ ?2.1?2auto_size, 0x40
 ## End main
.section .bss
.section .data
.section .data
.section .text
.import printf
.type printf,@function
.import atoi
.type atoi,@function
