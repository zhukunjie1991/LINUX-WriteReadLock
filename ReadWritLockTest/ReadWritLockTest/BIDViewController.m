//
//  BIDViewController.m
//  ReadWritLockTest
//
//  Created by zhukunjie on 14-6-10.
//  Copyright (c) 2014年 zhukunjie. All rights reserved.
//

#import "BIDViewController.h" 
#include "Mutex.h"
#define  Num 0
static int num=Num;
RWLock m_lock;
void *thread_function_read_o(void *arg);//读线程1
void *thread_function_read_t(void *arg);//读线程2
void *thread_function_write_o(void *arg);//写线程1
void *thread_function_read_o(void *arg)
{
    m_lock.ReadLock();
    printf("%d,this num is handled by%u,and read_2 thread %u\n",num,getpid(),
           (unsigned int)pthread_self());
    m_lock.UnlockRead();
    return (void*) NULL;
}
void *thread_function_read_t(void *arg)
{
    m_lock.ReadLock();
    printf("%d,this num is handled by%u,and read_2 thread %u\n",num,getpid(),
           (unsigned int)pthread_self());
    m_lock.UnlockRead();
    return (void*) NULL;
}
void *thread_function_write_o(void *arg)
{
    m_lock.WriteLock();
    printf("%d,this num is handled by%u,and write_1 thread %u\n",num++,getpid(),
           (unsigned int)pthread_self());
    m_lock.UnlockWrite();
    return (void*) NULL;
}

@interface BIDViewController ()

@end
@implementation BIDViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)Thread:(id)sender {
     pthread_t tid1,tid2,tid3;
    while(num <10)    //create the threads in the loop
    {
        pthread_create(&tid1,NULL,thread_function_write_o,&num);
        usleep(1000);           //approximately about 1ms in the interval of creation of thread
        pthread_create(&tid2,NULL,thread_function_read_t,&num);
        usleep(1000);
        pthread_create(&tid3,NULL,thread_function_read_o,&num);
        usleep(1000);
    }
    m_lock.~RWLock();
    
}
@end
