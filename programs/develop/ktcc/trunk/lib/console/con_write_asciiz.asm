format ELF

include "__lib__.inc"

fun      equ con_write_asciiz
fun_str  equ 'con_write_asciiz'

section '.text'

fun_name db fun_str, 0

section '.data'

extrn lib_name
public fun

fun dd fun_name
lib dd lib_name
