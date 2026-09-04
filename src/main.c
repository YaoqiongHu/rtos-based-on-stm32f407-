#include "rtos.h"
#include "gpio.h"
#include "clk.h"
#include "mutex.h"

/* =====================================================================
 * mutex(互斥量)功能测试 —— 现象全部走 IO 口,逻辑分析仪观察
 *
 * 优先级布局(已扩到 4 级):0=最高 ... 3=最低(idle)
 *   控制器/任务H = prio0   XM任务/Y任务 = prio1   L任务 = prio2   idle = prio3
 *
 * 阶段(每 5 秒切换一轮,循环往复):
 *   阶段 0 = 互斥 + 阻塞:任务X、Y(同 prio1)争抢 m_mutex0
 *   阶段 1 = 优先级继承:低优先级 L 持锁忙,中优先级 M 抢 CPU(反转),
 *            高优先级 H 周期抢锁(触发继承,L 被抬升,快速让位)
 *
 * 引脚:
 *   PA8  阶段0: 任务X 的临界区脉冲(持锁期间为高)
 *   PA9  阶段0: 任务Y 的临界区脉冲
 *   PA6  阶段1: L 的临界区脉冲
 *   PC7  阶段1: H 每次成功拿到锁的短脉冲(判继承是否生效)
 *   PC8  阶段1: M(中优先级)的运行指示方波(被抬升的 L 抢占时暂停)
 * ===================================================================== */

static volatile uint32 test_phase = 0;   /* 0=互斥 1=继承 */
static mutex* m_mutex0;                  /* 阶段0:互斥测试用 */
static mutex* m_mutex1;                  /* 阶段1:优先级继承用 */

/* 粗略忙循环延时:不精确,仅供演示(168MHz 下 ~1ms ≈ 4 万次循环) */
static void busy_ms(uint32 ms)
{
    volatile uint32 n = ms * 40000UL;
    while (n--) ;
}

/* ---------- 空闲任务:最低优先级兜底 ---------- */
static void idle_task(void)
{
    while(1) { }
}

/* ---------- 阶段控制器(prio0):每阶段 5 秒 ---------- */
static void task_controller(void)
{
    while(1)
    {
        test_phase = 0;  rtos_delay(5000);
        test_phase = 1;  rtos_delay(5000);
    }
}

/* ---------- 任务 XM(prio1):阶段0 扮演"互斥任务X";阶段1 扮演"中优先级M" ----------
 * 阶段0: 和 Y 抢 m_mutex0,临界区脉冲 PA8 —— 与 PA9 永不重叠
 * 阶段1: M:忙 200ms(期间 PC8 翻转作运行指示)睡 200ms —— 忙时抢占低优先级 L(反转) */
static void task_XM(void)
{
    while(1)
    {
        if (test_phase == 0)                    /* 互斥任务 X */
        {
            mutex_take(m_mutex0);
            gpio_write(GPIOA, GPIO_PIN_8, 1);
            busy_ms(120);
            gpio_write(GPIOA, GPIO_PIN_8, 0);
            mutex_give(m_mutex0);
            rtos_delay(30);
        }
        else if (test_phase == 1)               /* 中优先级 M */
        {
            uint32 t = 200;
            while (t--)                         /* 忙 200ms:每次翻转 PC8 约耗 1ms */
            {
                gpio_toggle(GPIOC, GPIO_PIN_8);
                busy_ms(1);
            }
            rtos_delay(200);                    /* 睡 200ms,给 L 留执行窗口 */
        }
        else
        {
            rtos_delay(5);
        }
    }
}

/* ---------- 任务 Y(prio1):阶段0 互斥任务 ---------- */
static void task_Y(void)
{
    while(1)
    {
        if (test_phase != 0)
        {
            rtos_delay(5);
            continue;
        }
        mutex_take(m_mutex0);
        gpio_write(GPIOA, GPIO_PIN_9, 1);
        busy_ms(120);
        gpio_write(GPIOA, GPIO_PIN_9, 0);
        mutex_give(m_mutex0);
        rtos_delay(30);
    }
}

/* ---------- 任务 L(prio2,低):阶段1 持锁者 ----------
 * 持锁做 350ms 长忙。若被中优先级 M 抢占 → 反转;H 来抢锁 → 继承抬升 L,
 * L 抢过 M 快速跑完 → give → H 拿到锁 */
static void task_L(void)
{
    while(1)
    {
        if (test_phase != 1)
        {
            rtos_delay(5);
            continue;
        }
        mutex_take(m_mutex1);
        gpio_write(GPIOA, GPIO_PIN_6, 1);       /* 进临界区(PA6) */
        busy_ms(350);
        gpio_write(GPIOA, GPIO_PIN_6, 0);       /* 出临界区 */
        mutex_give(m_mutex1);
        rtos_delay(150);
    }
}

/* ---------- 任务 H(prio0,高):阶段1 每 700ms 抢一次锁 ----------
 * 若撞上 L 持锁 → 阻塞等待 → 继承生效 → 短暂等待后拿到锁(PC7 短脉冲)。
 * PC7 脉冲稳定出现 = 锁等待有界 = 优先级继承有效 */
static void task_H(void)
{
    while(1)
    {
        if (test_phase != 1)
        {
            rtos_delay(20);
            continue;
        }
        mutex_take(m_mutex1);
        gpio_write(GPIOC, GPIO_PIN_7, 1);
        busy_ms(10);
        gpio_write(GPIOC, GPIO_PIN_7, 0);
        mutex_give(m_mutex1);
        rtos_delay(700);
    }
}

int main(void)
{
    rtos_init();

    RCC_GPIOA_CLK_ENABLE();
    RCC_GPIOC_CLK_ENABLE();
    gpio_init_out(GPIOA, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_init_out(GPIOC, GPIO_PIN_7 | GPIO_PIN_8);

    m_mutex0 = mutex_create();                 /* 阶段0:互斥 */
    m_mutex1 = mutex_create();                 /* 阶段1:继承 */

    rtos_create_task(idle_task,     3, 0);     /* idle 最低优先级 */
    rtos_create_task(task_controller, 0, 1);//
    rtos_create_task(task_H,        0, 2);//
    rtos_create_task(task_XM,       1, 3);//
    rtos_create_task(task_Y,        1, 4);//
    rtos_create_task(task_L,        2, 5);

    rtos_start();
    while(1);
}
