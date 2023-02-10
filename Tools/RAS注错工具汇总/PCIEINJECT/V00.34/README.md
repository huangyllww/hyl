GPP桥下接设备建议选择Intel Corporation I350 Gigabit Network Connection或
Intel Corporation I210 Gigabit Network Connection

注意：由于运行App修改了错误上报相关的配置，建议测试完毕后重启系统恢复环境！！！

1、update-pciids.sh 只需要运行一次，这个脚本是更新PCIe deviceid和vendorid的，如果不更新，
/rastest alldev中name 列无法把deviceid转换成字符串，只能显示成十六进制数，不方便阅读。

2、rastest 目前提供下面几个功能。由于要读写reg，APP需要再root权限下才可以运行。

下面三个命令可以显示帮助
./rastest -help
./rastest -h
./rastest

2.0 功能0：显示系统中所有的设备
命令./rastest alldev

2.1 功能1：显示系统中所有的海光和AMD的设备
命令./rastest hygondev

2.2 功能2：显示系统中所有的海光和AMD的GPP bridge信息，显示系统中有几个socket，每个socket几个die
并显示每个GPP桥后面接的sub device（如果有设备接在GPP桥后面的话）
命令./rastest gpp

2.3 功能3：显示支持注入的错误类型和error report的方式
命令./rastest pcie_err_info

2.4 功能4：往GPP bridge端口注入错误
命令./rastest pcie_err -t [pcie errtype] -s [bus:dev.func] -e [error report] -d [disable dpc] -c [rety cnt] -i [interval].
其中 errtype 支持14种错误：lcrc_tx/lcrc_rx/ecrc_tx/ecrc_rx/acs_fatal/acs_nonfatal/completion_timeout
unexpected_cmplt/malformed_tlp/ecrc/unsupported_req/bad_tlp/bad_dllp/advisory_nonfatal

-e [error report] 可以选择mask错误上报（参数mask_err_report），这样app会读取错误状态，并打印在终端
如果unmask错误上报(参数mask_err_report)，根据系统配置，错误可能上报给OS（即OS native model），也可能上报给BIOS（即firmware first model）
由于unmask错误上报，依赖于系统配置，可能上报不成功或者不容易观察，错误上报机制是客户选择，不在app可以管理的范围，
建议使用mask_err_report，让APP把错误打印在终端

由于有些错误，BIOS配置ras action时配置成了syncflood，会导致系统重启，建议在运行APP注入错误前
先使用脚本读一下对应die的（第一个参数）ras action的配置./iohc_ras.sh dieid read
如果对应错误被配置成了syncflood，则使用脚本把对应die的（第一个参数）ras action清零 ./iohc_ras.sh dieid write
如果客户本身就像观察注入对应的错误是否会到syncflood，则不需要运行脚本
-d [disable dpc]是为了禁止DPC功能，由于某些OS版本会使能DPC功能，当注入PCIe错误时可能导致GPP桥下面设备被删除，无法重复注入。-d 1就是禁止GPP桥的DPC功能。
-c [rety cnt] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试次数，范围3-5次）
-i [interval] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试的时间间隔，范围1-3秒）

./rastest pcie_err -t lcrc_rx -s 0:1.3 -e mask_err_report -d 1-c 5 -i 3
导致Root Port 报bad tlp的错误

./rastest pcie_err -t lcrc_tx -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port对端设备报bad tlp的错误

./rastest pcie_err -t ecrc_rx -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root Port 报ecrc error的错误

./rastest pcie_err -t ecrc_tx -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port下面最远端设备报ecrc error的错误

./rastest pcie_err -t acs_fatal -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root Port 报ACS Violation（severity为fatal）的错误

./rastest pcie_err -t acs_nonfatal -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报ACS Violation（severity为nonfatal）的错误

./rastest pcie_err -t completion_timeout -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报completion timeout的错误

./rastest pcie_err -t unexpected_cmplt -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报unexpected completion的错误

./rastest pcie_err -t malformed_tlp -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报malformed tlp的错误

./rastest pcie_err -t ecrc -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报ecrc的错误

./rastest pcie_err -t unsupported_req -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报unsupported request的错误（注意需要GPP桥下面的设备发送一笔memory read或者memory write的请求）

./rastest pcie_err -t bad_tlp -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报bad tlp的错误

./rastest pcie_err -t bad_dllp -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报bad dllp的错误(注意出错DLLP包可能是ACK、NACK包，也可能是updata flow ctrl等，DLLP丢失，导致不可预知的错误)
./rastest pcie_err -t advisory_nonfatal -s 0:1.3 -e mask_err_report -d 1 -c 5 -i 3
导致Root port报advisory non fatal的错误

2.5 功能5显示NBIO SRAM的信息
命令./rastest nbio_sram_info

2.6 功能6往NBIO SRAM注入错误
命令 ./rastest nbio_sram_inject -d [socket:die] -t [nbio errtype] -g [groupid] -s [structid] -l [err report threshold] -c [rety cnt] -i [interval].

-d [socket:die] 真实的socketid和dieid，见./rastest nbio_sram_info输出的socketid和dieid列
-t [nbio errtype] NBIO SRAM error类型，见 ./rastest nbio_sram_info输出的error type:
-g [groupid] groupid，见./rastest nbio_sram_info输出的groupid列，和structureid一起指定唯一的SRAM
-s [structid] structid，见./rastest nbio_sram_info输出的structureid列，和groupid一起指定唯一的SRAM
-c [rety cnt] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试次数，范围3-5次）
-i [interval] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试的时间间隔，范围1-3秒）

2.7 功能7 显示GMI/xGMI/WAFL的信息和支持注入的错误类型
命令 ./rastest_dbg gmi_xgmi_wafl_info

2.8 功能8 GMI/xGMI/WAFL 链路上注入错误
命令 ./rastest_dbg gmi_xgmi_wafl_inject -d [socket:die] -l [link type] -n [linkid] -t [link errtype] -e [err report] -c [rety cnt] -i [interval].

-d [socket:die] 真实的socketid和dieid，见./rastest_dbg gmi_xgmi_wafl_info输出的socketid和dieid列
-t [nbio errtype] NBIO SRAM error类型，见 ./rastest_dbg gmi_xgmi_wafl_info输出的link type: GMI/xGMI/WAFL三种
-n [linkid] GMI/xGMI/WAFL的编号，见 ./rastest_dbg gmi_xgmi_wafl_info输出的GMI/xGMI/WAFL编号。注意：只有linkup的编号才可以注入错误
-t [link errtype] link error type，见./rastest_dbg gmi_xgmi_wafl_info输出的error type，目前支持DataParityErr、ReplayFifoOverflowErr、ReplayFifoUnderflowErr、ElasticFifoOverflowErr
-e [err report]  error report控制标志，见./rastest_dbg gmi_xgmi_wafl_info输出的link error report ctrl。
unmask_err_report-----错误上报，可能导致syncflood
mask_err_report-----错误不上报，GMI的某些错误可能导致syncflood
-c [rety cnt] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试次数，范围3-5次）
-i [interval] 注入不成功情况下（只有maks error report才能检查是否注入成功），重试的时间间隔，范围1-3秒）