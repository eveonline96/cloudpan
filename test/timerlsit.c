#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

struct timer_list timerlist;

void timer_fn(unsigned long data)
{

}
static int __init init(void)
{
	init_timer(&timerlist);   		//初始化时间
	timerlist.expires=jiffies+HZ;   //设置定时到期时间
	timerlist.function=timer_fn;    //设置中断处理函数
	timerlsit.data=0;               //
	add_timer(&timerlist);          //将定时器加入系统
	return 0;
}
static void __exit exit(void)
{

}
module_init(init);
module_exit(exit);
MODULE_LICENSE("GPL");