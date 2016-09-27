
//
//  main.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include <stdio.h>
//#include "scheduler.c"
#include "include/cthread.h"
//#define _XOPEN_SOURCE 600 // Resolve um problema do OS X com ucontext.h


int main() {
    // insert code here...
    printf("Hello, World!\n");
    

    int argc = 12;
    
    int tid = ccreate(&func0, (void*) argc);
    if (tid == ERROR) {
        
    } else {
    
    }
    return 0;
}