//此程式由AI生成
//每兩秒用printk函式輸出訊息
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h> // 包含 jiffies 相關定義

static struct timer_list my_timer;
static int count = 0;

// 1. 定時器回呼函式
void my_timer_callback(struct timer_list *t) {
    count++;
    printk(KERN_INFO "Timer Demo: 這是第 %d 次輸出，目前時間戳: %lu\n", count, jiffies);

    // 重新註冊定時器，實現「每隔一段時間」循環執行
    // HZ 代表一秒鐘的滴答數，所以 HZ * 2 代表兩秒後再次執行
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000)); 
}

// 2. 模組初始化
static int __init my_init(void) {
    printk(KERN_INFO "Timer Demo: 模組已載入\n");

    // 初始化定時器結構
    timer_setup(&my_timer, my_timer_callback, 0);

    // 設定定時器在 2 秒後觸發 (jiffies 是系統目前的滴答計數)
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    return 0;
}

// 3. 模組退出
static void __exit my_exit(void) {
    // 非常重要：卸載前必須停止定時器
    del_timer(&my_timer);
    printk(KERN_INFO "Timer Demo: 模組已卸載，定時器已停止\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
