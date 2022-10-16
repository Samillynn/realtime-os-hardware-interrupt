.macro make_svc func_name svc_code
.text
.align 4
.global \func_name
\func_name:
svc #\svc_code
        ret
.endm

make_svc Create      1
make_svc Yield       2
make_svc Exit        3
make_svc MyTid       11
make_svc MyParentTid 12
make_svc Send        21
make_svc Receive     22
make_svc Reply       23