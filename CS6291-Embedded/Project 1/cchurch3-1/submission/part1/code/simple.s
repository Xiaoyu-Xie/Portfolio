 ## Target: VEX 1 cluster (big endian)
.comment ""
.comment "Copyright (C) 1990-2010 Hewlett-Packard Company"
.comment "VEX C compiler version 3.43 (20110131 release)"
.comment ""
.comment "-dir /home/vagrant/vex-3.43 -ms -fmm=myvex.mm -fno-xnop"
.sversion 3.43
.rta 2
.section .bss
.align 32
.section .data
.align 32
.equ _?1TEMPLATEPACKET.1, 0x0
 ## Begin main
.section .text
.proc
.entry caller, sp=$r0.1, rl=$l0.0, asize=-64, arg($r0.3:s32,$r0.4:u32)
main::
.trace 1
	c0    add $r0.1 = $r0.1, (-0x40)
;;								## 0
	c0    stw 0x10[$r0.1] = $l0.0  ## spill ## t78
;;								## 1
	c0    stw 0x14[$r0.1] = $r0.4  ## spill ## t92
;;								## 2
	c0    ldw $r0.3 = 4[$r0.4]   ## bblock 0, line 7, t3, t92
;;								## 3
;;								## 4
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 0, line 7,  raddr(atoi)(P32),  t3
;;								## 5
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 6
;;								## 7
;;								## 8
	c0    ldw $r0.3 = 8[$r0.4]   ## bblock 1, line 8, t7, t92
;;								## 9
;;								## 10
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 1, line 8,  raddr(atoi)(P32),  t7
;;								## 11
	c0    stw 0x18[$r0.1] = $r0.3  ## spill ## t4
;;								## 12
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 13
;;								## 14
;;								## 15
	c0    ldw $r0.3 = 12[$r0.4]   ## bblock 2, line 9, t11, t92
;;								## 16
;;								## 17
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 2, line 9,  raddr(atoi)(P32),  t11
;;								## 18
	c0    stw 0x1c[$r0.1] = $r0.3  ## spill ## t8
;;								## 19
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 20
;;								## 21
;;								## 22
	c0    ldw $r0.3 = 16[$r0.4]   ## bblock 3, line 10, t15, t92
;;								## 23
;;								## 24
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 3, line 10,  raddr(atoi)(P32),  t15
;;								## 25
	c0    stw 0x20[$r0.1] = $r0.3  ## spill ## t12
;;								## 26
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 27
;;								## 28
;;								## 29
	c0    ldw $r0.3 = 20[$r0.4]   ## bblock 4, line 11, t19, t92
;;								## 30
;;								## 31
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 4, line 11,  raddr(atoi)(P32),  t19
;;								## 32
	c0    stw 0x24[$r0.1] = $r0.3  ## spill ## t16
;;								## 33
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 34
;;								## 35
;;								## 36
	c0    ldw $r0.3 = 24[$r0.4]   ## bblock 5, line 12, t23, t92
;;								## 37
;;								## 38
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 5, line 12,  raddr(atoi)(P32),  t23
;;								## 39
	c0    stw 0x28[$r0.1] = $r0.3  ## spill ## t20
;;								## 40
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 41
;;								## 42
;;								## 43
	c0    ldw $r0.3 = 28[$r0.4]   ## bblock 6, line 13, t27, t92
;;								## 44
;;								## 45
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 6, line 13,  raddr(atoi)(P32),  t27
;;								## 46
	c0    stw 0x2c[$r0.1] = $r0.3  ## spill ## t24
;;								## 47
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 48
;;								## 49
;;								## 50
	c0    ldw $r0.3 = 32[$r0.4]   ## bblock 7, line 14, t31, t92
;;								## 51
;;								## 52
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 7, line 14,  raddr(atoi)(P32),  t31
;;								## 53
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 54
;;								## 55
;;								## 56
	c0    ldw $r0.3 = 36[$r0.4]   ## bblock 8, line 15, t35, t92
;;								## 57
;;								## 58
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 8, line 15,  raddr(atoi)(P32),  t35
;;								## 59
	c0    stw 0x30[$r0.1] = $r0.3  ## spill ## t32
;;								## 60
	c0    ldw $r0.4 = 0x14[$r0.1]  ## restore ## t92
;;								## 61
;;								## 62
;;								## 63
	c0    ldw $r0.3 = 40[$r0.4]   ## bblock 9, line 16, t39, t92
;;								## 64
;;								## 65
.call atoi, caller, arg($r0.3:u32), ret($r0.3:s32)
	c0    call $l0.0 = atoi   ## bblock 9, line 16,  raddr(atoi)(P32),  t39
;;								## 66
	c0    ldw $r0.2 = 0x24[$r0.1]  ## restore ## t16
;;								## 67
	c0    ldw $r0.5 = 0x28[$r0.1]  ## restore ## t20
;;								## 68
	c0    ldw $r0.6 = 0x20[$r0.1]  ## restore ## t12
;;								## 69
	c0    ldw $r0.7 = 0x2c[$r0.1]  ## restore ## t24
;;								## 70
	c0    mpylu $r0.8 = $r0.2, $r0.5   ## bblock 10, line 19,  t109,  t16,  t20
;;								## 71
	c0    mpyhs $r0.9 = $r0.2, $r0.5   ## bblock 10, line 19,  t110,  t16,  t20
;;								## 72
	c0    add $r0.7 = $r0.6, $r0.7   ## bblock 10, line 21,  t107,  t12,  t24
;;								## 73
	c0    add $r0.8 = $r0.8, $r0.9   ## bblock 10, line 19,  t53,  t109,  t110
;;								## 74
	c0    add $r0.8 = $r0.8, $r0.7   ## bblock 10, line 21,  t113,  t53,  t107
;;								## 75
	c0    ldw $r0.9 = 0x1c[$r0.1]  ## restore ## t8
;;								## 76
	c0    add $r0.7 = $r0.2, $r0.7   ## bblock 10, line 17,  t108,  t16,  t107
;;								## 77
	c0    ldw $r0.10 = 0x18[$r0.1]  ## restore ## t4
;;								## 78
	c0    mpylu $r0.11 = $r0.9, $r0.6   ## bblock 10, line 20,  t111,  t8,  t12
;;								## 79
	c0    mpyhs $r0.6 = $r0.9, $r0.6   ## bblock 10, line 20,  t112,  t8,  t12
;;								## 80
	c0    add $r0.10 = $r0.10, $r0.7   ## bblock 10, line 23,  t114,  t4,  t108
;;								## 81
	c0    add $r0.11 = $r0.11, $r0.6   ## bblock 10, line 20,  t58,  t111,  t112
;;								## 82
	c0    add $r0.8 = $r0.8, $r0.11   ## bblock 10, line 21,  t106,  t113,  t58
;;								## 83
	c0    add $r0.2 = $r0.2, $r0.8   ## bblock 10, line 23,  t116,  t16,  t106
;;								## 84
	c0    add $r0.5 = $r0.5, $r0.8   ## bblock 10, line 23,  t117,  t20,  t106
;;								## 85
	c0    add $r0.2 = $r0.2, $r0.5   ## bblock 10, line 23,  t122,  t116,  t117
;;								## 86
	c0    ldw $r0.5 = 0x30[$r0.1]  ## restore ## t32
;;								## 87
	c0    add $r0.9 = $r0.9, $r0.7   ## bblock 10, line 23,  t115,  t8,  t108
;;								## 88
;;								## 89
	c0    add $r0.6 = $r0.5, $r0.3   ## bblock 10, line 23,  t119,  t32,  t36
;;								## 90
	c0    add $r0.10 = $r0.10, $r0.6   ## bblock 10, line 23,  t120,  t114,  t119
;;								## 91
	c0    add $r0.10 = $r0.10, $r0.2   ## bblock 10, line 23,  t123,  t120,  t122
;;								## 92
	c0    add $r0.5 = $r0.5, $r0.3   ## bblock 10, line 23,  t118,  t32,  t36
;;								## 93
	c0    add $r0.9 = $r0.9, $r0.5   ## bblock 10, line 23,  t121,  t115,  t118
;;								## 94
	c0    add $r0.4 = $r0.10, $r0.9   ## bblock 10, line 23,  t77,  t123,  t121
;;								## 95
	c0    mov $r0.3 = (_?1STRINGPACKET.1 + 0)   ## addr(_?1STRINGVAR.1)(P32)
;;								## 96
.call printf, caller, arg($r0.3:u32,$r0.4:s32), ret($r0.3:s32)
	c0    call $l0.0 = printf   ## bblock 10, line 23,  raddr(printf)(P32),  addr(_?1STRINGVAR.1)(P32),  t77
;;								## 97
	c0    ldw $l0.0 = 0x10[$r0.1]  ## restore ## t78
;;								## 98
;;								## 99
;;								## 100
;;								## 101
.return ret()
	c0    return $r0.1 = $r0.1, (0x40), $l0.0   ## bblock 11, line 24,  t79,  ?2.1?2auto_size(I32),  t78
;;								## 102
.endp
.section .bss
.section .data
_?1STRINGPACKET.1:
    .data1 37
    .data1 100
    .data1 10
    .data1 0
.equ ?2.1?2scratch.0, 0x0
.equ ?2.1?2ras_p, 0x10
.equ ?2.1?2spill_p, 0x14
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
