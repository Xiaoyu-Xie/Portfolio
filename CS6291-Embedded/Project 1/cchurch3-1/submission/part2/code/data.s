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
	c0    call $l0.0 = atoi   ## bblock 5, line 11,  raddr(atoi)(P32),  t23
;;								## 36
	c0    ldw $r0.2 = 0x10[$r0.1]   ## bblock 6, line 13, t24, t86
;;								## 37
	c0    add $r0.5 = $r0.1, 0x18   ## bblock 6, line 15,  t106,  t86,  offset(a4?1.2)=0x18(P32)
;;								## 38
	c0    add $r0.6 = $r0.1, 0x1c   ## bblock 6, line 18,  t108,  t86,  offset(a5?1.2)=0x1c(P32)
;;								## 39
	c0    cmpgt $b0.0 = $r0.2, 10   ## bblock 6, line 13,  t104(I1),  t24,  10(SI32)
;;								## 40
	c0    slct $r0.5 = $b0.0, $r0.5, $r0.6   ## bblock 6, line 15,  t100,  t104(I1),  t106,  t108
;;								## 41
	c0    ldw $r0.2 = 0x18[$r0.1]   ## bblock 6, line 23, t37, t86
;;								## 42
	c0    ldw $r0.6 = 0x14[$r0.1]   ## bblock 6, line 22, t31, t86
;;								## 43
	c0    add $r0.7 = $r0.1, 0x14   ## bblock 6, line 14,  t105,  t86,  offset(a2?1.2)=0x14(P32)
;;								## 44
	c0    add $r0.8 = $r0.3, $r0.2   ## bblock 6, line 24,  t38,  t20,  t37
;;								## 45
	c0    add $r0.6 = $r0.6, 5   ## bblock 6, line 22,  t34,  t31,  5(SI32)
;;								## 46
	c0    add $r0.2 = $r0.2, $r0.6   ## bblock 6, line 23,  t109,  t37,  t34
;;								## 47
	c0    add $r0.9 = $r0.1, 0x10   ## bblock 6, line 17,  t107,  t86,  offset(a0?1.2)=0x10(P32)
;;								## 48
	c0    slct $r0.7 = $b0.0, $r0.7, $r0.9   ## bblock 6, line 14,  t101,  t104(I1),  t105,  t107
;;								## 49
	c0    ldw $r0.9 = 0x28[$r0.1]  ## restore ## t4
;;								## 50
	c0    add $r0.10 = $r0.2, -20   ## bblock 6, line 33,  t112,  t109,  -20(SI32)
;;								## 51
	c0    stw 0x1c[$r0.1] = $r0.3   ## bblock 6, line 11, t86, t20
;;								## 52
	c0    mov $r0.3 = (_?1STRINGPACKET.1 + 0)   ## addr(_?1STRINGVAR.1)(P32)
;;								## 53
	c0    stw 0x14[$r0.1] = $r0.8   ## bblock 6, line 24, t86, t38
;;								## 54
	c0    stw 0x10[$r0.1] = $r0.6   ## bblock 6, line 22, t86, t34
;;								## 55
	c0    ldw $r0.6 = 0[$r0.5]   ## bblock 6, line 25, t41, t100
;;								## 56
;;								## 57
;;								## 58
	c0    add $r0.6 = $r0.6, $r0.2   ## bblock 6, line 25,  t42,  t41,  t109
;;								## 59
	c0    stw 0[$r0.7] = $r0.6   ## bblock 6, line 25, t101, t42
;;								## 60
	c0    ldw $r0.6 = 0[$r0.5]   ## bblock 6, line 26, t47, t100
;;								## 61
	c0    ldw $r0.8 = 0x10[$r0.1]   ## bblock 6, line 26, t45, t86
;;								## 62
;;								## 63
	c0    add $r0.9 = $r0.9, $r0.6   ## bblock 6, line 26,  t110,  t4,  t47
;;								## 64
	c0    add $r0.9 = $r0.9, $r0.8   ## bblock 6, line 26,  t49,  t110,  t45
;;								## 65
	c0    stw 0x14[$r0.1] = $r0.9   ## bblock 6, line 26, t86, t49
;;								## 66
	c0    ldw $r0.6 = 0[$r0.7]   ## bblock 6, line 27, t51, t101
;;								## 67
;;								## 68
;;								## 69
	c0    add $r0.9 = $r0.9, $r0.6   ## bblock 6, line 27,  t56,  t49,  t51
;;								## 70
	c0    stw 0[$r0.5] = $r0.9   ## bblock 6, line 27, t100, t56
;;								## 71
	c0    ldw $r0.6 = 0x18[$r0.1]   ## bblock 6, line 28, t63, t86
;;								## 72
	c0    ldw $r0.8 = 0x14[$r0.1]   ## bblock 6, line 31, t81, t86
;;								## 73
	c0    ldw $r0.11 = 0x10[$r0.1]   ## bblock 6, line 33, t83, t86
;;								## 74
	c0    add $r0.9 = $r0.9, $r0.6   ## bblock 6, line 28,  t78,  t56,  t63
;;								## 75
	c0    add $r0.10 = $r0.10, $r0.8   ## bblock 6, line 33,  t117,  t112,  t81
;;								## 76
	c0    add $r0.2 = $r0.2, $r0.11   ## bblock 6, line 33,  t113,  t109,  t83
;;								## 77
	c0    stw 0x1c[$r0.1] = $r0.9   ## bblock 6, line 28, t86, t78
;;								## 78
	c0    ldw $r0.11 = 0[$r0.7]   ## bblock 6, line 29, t69, t101
;;								## 79
	c0    ldw $r0.12 = 0[$r0.5]   ## bblock 6, line 31, t67, t100
;;								## 80
;;								## 81
	c0    add $r0.8 = $r0.11, $r0.8   ## bblock 6, line 31,  t111,  t69,  t81
;;								## 82
	c0    add $r0.8 = $r0.8, $r0.12   ## bblock 6, line 31,  t79,  t111,  t67
;;								## 83
	c0    add $r0.11 = $r0.11, $r0.8   ## bblock 6, line 33,  t116,  t69,  t79
;;								## 84
	c0    add $r0.2 = $r0.2, $r0.11   ## bblock 6, line 33,  t118,  t113,  t116
;;								## 85
	c0    stw 0x18[$r0.1] = $r0.8   ## bblock 6, line 31, t86, t79
;;								## 86
	c0    ldw $r0.7 = 0[$r0.7]   ## bblock 6, line 33, t77, t101
;;								## 87
	c0    ldw $r0.5 = 0[$r0.5]   ## bblock 6, line 33, t75, t100
;;								## 88
;;								## 89
	c0    add $r0.6 = $r0.6, $r0.7   ## bblock 6, line 33,  t114,  t63,  t77
;;								## 90
	c0    add $r0.9 = $r0.9, $r0.5   ## bblock 6, line 33,  t115,  t78,  t75
;;								## 91
	c0    add $r0.6 = $r0.6, $r0.9   ## bblock 6, line 33,  t119,  t114,  t115
;;								## 92
	c0    add $r0.10 = $r0.10, $r0.6   ## bblock 6, line 33,  t120,  t117,  t119
;;								## 93
	c0    add $r0.4 = $r0.10, $r0.2   ## bblock 6, line 33,  t84,  t120,  t118
;;								## 94
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
