//此程式由AI生成
//可以將訊息寫入buffer，讀取時就會顯示該訊息
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // 包含 copy_to_user 和 copy_from_user

#define DEVICE_NAME "hello_dev"
#define BUF_SIZE 1024

static int major_num;
static char kernel_buffer[BUF_SIZE]; // 核心內部的緩衝區
static int data_size = 0;            // 目前儲存的資料長度

// --- Read 函式：將核心緩衝區傳給使用者 ---
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    if (*offset >= data_size) return 0; // 讀完了

    if (len > data_size - *offset) {
        len = data_size - *offset;
    }

    if (copy_to_user(buffer, kernel_buffer + *offset, len)) {
        return -EFAULT;
    }

    *offset += len;
    return len;
}

// --- Write 函式：接收使用者傳來的資料 ---
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    // 檢查寫入長度是否超過核心緩衝區大小
    if (len > BUF_SIZE - 1) {
        len = BUF_SIZE - 1;
    }

    // 將資料從使用者空間「拷貝」到核心空間 (重要安全檢查！)
    if (copy_from_user(kernel_buffer, buffer, len)) {
        return -EFAULT;
    }

    data_size = len;
    kernel_buffer[data_size] = '\0'; // 加上結尾符號，方便 printk 顯示

    printk(KERN_INFO "hello_dev: 收到來自使用者的 %zu 位元組資料\n", len);
    return len; // 回傳實際寫入的長度
}

// 註冊 file_operations
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write, // 新增這行
};

static int __init hello_init(void) {
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0) return major_num;
    printk(KERN_INFO "hello_dev: 載入成功，主裝置號為 %d\n", major_num);
    return 0;
}

static void __exit hello_exit(void) {
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "hello_dev: 卸載成功\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
