PROGRAM_NAME=$1
LOG_FILE="monitor-process.log"
pid=$(pgrep $PROGRAM_NAME)
MEM_USAGE_HEADER="PID    %CPU   %MEM    RSS    VSZ"

TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
# 获取进程的内存使用情况（RSS: 物理内存, VSZ: 虚拟内存）
MEM_USAGE=$(ps -C $PROGRAM_NAME -o pid,%cpu,%mem,rss,vsz --no-headers)

echo "$MEM_USAGE_HEADER" > $LOG_FILE

while true; do
    pid=$(pgrep $PROGRAM_NAME)

    # 检查是否找到进程ID
    if [ -z "$pid" ]; then
        echo "Process $PROGRAM_NAME not found."
        exit 1
    fi
    
    echo "$TIMESTAMP $MEM_USAGE" >> $LOG_FILE

    sleep 30
done%