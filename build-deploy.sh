#!/bin/bash
# build and deploy moboot via novacom
make clean; make -j$(nproc) && echo "Press enter when ready to deploy..." && read enter && sleep 3 && novacom boot mem:// < build-pre3/lk.uImage
