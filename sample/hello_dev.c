//此程式由AI生成
//可以從CLI中讀取訊息

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>      // 包含 struct file_operations 的定義
#include <linux/uaccess.h> // 包含 copy_to_user

#define DEVICE_NAME "hello_dev"
static int major_num;
static char *msg = "Hello from Kernel!\n";  //會顯示的message

// 1. 實作 read 函式
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;
    // 如果已經讀完了，回傳 0 (EOF)
    if (*offset > 0) return 0;

    // 將資料從核心空間拷貝到使用者空間 (重要安全步驟！)
    if (copy_to_user(buffer, msg, strlen(msg))) {
        return -EFAULT;
    }

    bytes_read = strlen(msg);
    *offset += bytes_read; // 更新偏移量
    return bytes_read;
}

// 2. 定義 file_operations 結構
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read, // 將系統的 read 連結到我們的 dev_read
};

// 3. 模組載入時執行
static int __init hello_init(void) {
    major_num = register_chrdev(0, DEVICE_NAME, &fops); // 自動分配主裝置號
    if (major_num < 0) {
        printk(KERN_ALERT "註冊失敗！\n");
        return major_num;
    }
    printk(KERN_INFO "Hello 模組已載入，裝置號為 %d\n", major_num);
    return 0;
}

// 4. 模組卸載時執行
static void __exit hello_exit(void) {
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Hello 模組已卸載\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
