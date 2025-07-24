
- monitor_process.sh 一个简单的监控进程cpu和内存使用情况的脚本。
  
  程序在后台运行起来后，先 ps 或 top，查看一下进程的名称，然后运行脚本：`bash monitor_process.sh process_name`，建议后台运行, `nohup bash monitor_process.sh proces_name &`。该脚本默认会在脚本运行的当前目录下生成一个名为 `monitor-process.log` 的日志文件，间隔 30s 将被监控的程序的内存和cpu使用率写入到该日志文件。