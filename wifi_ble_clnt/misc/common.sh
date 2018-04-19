
dbg()
{
    echo "$@"
}

checkrc()
{
    local rc=$1
    shift
    if [ $rc -eq 0 ]; then
        dbg "$@ 成功"
    else
        dbg "$@ 失败"
    fi

    return $rc
}

install_desktop_icon()
{
    local name=$1
    local exec=$2
    local comment=$3

    local fn=~/Desktop/$name

    dbg "安裝$name快捷方式"
    > "$fn"
    echo "#!/usr/bin/env xdg-open" >> "$fn"
    echo "Name=$name" >> "$fn"
    echo "Comment=$comment" >> "$fn"
    echo "Exec=$exec" >> "$fn"
    echo "Icon=" >> "$fn"
    echo "Terminal=true" >> "$fn"
    echo "Type=Application" >> "$fn"

    chmod 0755 "$fn"
}

netif_up_with_dhcp()
{
    sudo ifconfig "$1" up
    sudo dhclient -4 "$1"
}

bring_up_wired_ifs()
{
    local task_cnt=0
    for interface in $(iwconfig 2>&1 | grep "no wireless" | awk '{print $1}' | grep -wv lo); do
        if ! ifconfig $interface | egrep -q "inet.*([0-9]{1,3}\.){3}[0-9]{1,3}"; then
            echo "$interface is not configured, try to bring it up"
            netif_up_with_dhcp $interface&
            pid=$!
            echo "child process id = $pid"
            child_processes="$child_processes $pid"
            ((task_cnt++))
        fi
    done

    if [ $task_cnt -eq 0 ]; then
        return
    fi

    sleep 5

    for pid in $child_processes; do
        if ps $pid > /dev/null; then
            pkill -P $pid
        else
            echo "process $pid already terminated"
        fi
    done
}
