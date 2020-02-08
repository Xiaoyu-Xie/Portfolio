#! /bin/sh
if [ "$#" -ne 10 ]; then
	echo "Usage: $0 [a list of 10 numbers]" >&2
	exit 1
fi
./simple $1 $2 $3 $4 $5 $6 $7 $8 $9 $10; \
echo "#######################"; \
echo "         STATS         "; \
echo "#######################"; \
head -1 ta.log.000; \
rm gmon* ta.log*
