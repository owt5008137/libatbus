#!/bin/sh

ADDRESS=12345679 ;
UNIT_SIZE=1024 ;
SHM_SIZE=67108864 ; # 64MB

if [ $# -gt 1 ]; then
    ADDRESS="$1";
fi

if [ $# -gt 2 ]; then
    UNIT_SIZE=$2 ;
fi

./benchmark_shm_channel_recv $ADDRESS $UNIT_SIZE $SHM_SIZE > recv.log 2>&1 &

sleep 2;

./benchmark_shm_channel_send $ADDRESS $UNIT_SIZE $SHM_SIZE > send.log 2>&1 &

