GPP桥下接设备建议选择Intel Corporation I350 Gigabit Network Connection或
Intel Corporation I210 Gigabit Network Connection

注意：由于运行App修改了错误上报相关的配置，建议测试完毕后重启系统恢复环境！！！

1、update-pciids.sh 只需要运行一次，这个脚本是更新PCIe deviceid和vendorid的，如果不更新，
/pcieinject alldev中name 列无法把deviceid转换成字符串，只能显示成十六进制数，不方便阅读。

2、pcieinject 目前提供4个功能。由于要读写reg，APP需要再root权限下才可以运行。

下面三个命令可以显示帮助
./pcieinject -help
./pcieinject -h
./pcieinject

功能1：显示系统中所有的设备
命令./pcieinject alldev

功能2：显示系统中所有的海光和AMD的设备
命令./pcieinject hygondev

功能3：显示系统中所有的海光和AMD的GPP bridge信息，显示系统中有几个socket，每个socket几个die
并显示每个GPP桥后面接的sub device（如果有设备接在GPP桥后面的话）
命令./pcieinject gpp

功能4：往GPP bridge端口注入错误
命令./pcieinject pcie_err -t errtype -s bus:dev.func -a action.
其中 errtype 支持lcrc_tx/lcrc_rx/ecrc_tx/ecrc_rx/acs_fatal/acs_nonfatal 6种错误
action 可以选择mask错误上报（参数mask_err_report），这样app会自己读出错误，并打印在终端
如果unmask错误上报，根据系统配置，错误可能上报给OS（即OS native model），也可能上报给BIOS（即firmware first model）
由于unmask错误上报，依赖于系统配置，可能上报不成功或者不容易观察，错误上报机制是客户选择，不在app可以管理的范围，
建议使用mask_err_report，让APP把错误打印在终端

由于有些错误，BIOS配置ras action时配置成了syncflood，会导致系统重启，建议在运行APP注入错误前
先使用脚本读一下对应die的（第一个参数）ras action的配置./iohc_ras.sh 0 read
如果对应错误被配置成了syncflood，则使用脚本把对应die的（第一个参数）ras action清零 ./iohc_ras.sh 0 write
如果客户本身就像观察注入对应的错误是否会到syncflood，则不需要运行脚本

./pcieinject pcie_err -t lcrc_rx -s 0:1.3 -a mask_err_report
lcrc-rx 导致Root Port 报bad tlp的错误

./pcieinject pcie_err -t lcrc_tx -s 0:1.3 -a mask_err_report
lcrc-tx会导致Root port对端设备报bad tlp的错误

./pcieinject pcie_err -t ecrc_rx -s 0:1.3 -a mask_err_report
lcrc-rx 导致Root Port 报ecrc error的错误

./pcieinject pcie_err -t ecrc_tx -s 0:1.3 -a mask_err_report
lcrc-tx会导致Root port下面最远端设备报ecrc error的错误

./pcieinject pcie_err -t acs_fatal -s 0:1.3 -a mask_err_report
lcrc-rx 导致Root Port 报ACS Violation（severity为fatal）的错误

./pcieinject pcie_err -t acs_nonfatal -s 0:1.3 -a mask_err_report
lcrc-tx会导致Root port报ACS Violation（severity为nonfatal）的错误