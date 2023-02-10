#!/bin/sh
mpi_options="--allow-run-as-root --mca mpi_leave_pinned 1  --bind-to none --report-bindings --mca btl self,vader"
sudo /opt/openmpi/bin/mpirun $mpi_options -app ./appfile_ccx

