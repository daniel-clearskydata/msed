/* C:B**************************************************************************
This software is Copyright 2014,2015 Michael Romeo <r0m30@r0m30.com>

This file is part of msed.

msed is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

msed is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with msed.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */
#include "os.h"
#include "UnlockSEDs.h"
#include "MsedDevGeneric.h"
#include "MsedDevOpal1.h"
#include "MsedDevOpal2.h"

#include <ncurses.h>

uint8_t UnlockSEDs(char * password) {
/* Loop through drives */
    char devref[25];
    int i = 0;
    int failed = 0;
    MsedDev *tempDev;
    MsedDev *d;
    LOG(D4) << "Enter UnlockSEDs";
    mvprintw(6,2,"Scanning....");
    while (TRUE) {
        snprintf(devref,23,"/dev/sd%c",(char) 0x61+i);
         i += 1;
	tempDev = new MsedDevGeneric(devref);
	if (!tempDev->isPresent()) {break;}
        if ((!tempDev->isOpal1()) && (!tempDev->isOpal2())) {
             mvprintw(7+i,2,"Drive %s not supported  ",devref);
             delete tempDev;
             continue;
        }
        if (tempDev->isOpal2())
            d = new MsedDevOpal2(devref);
        else
            d = new MsedDevOpal1(devref);
        delete tempDev;
        failed = 0;
	mvprintw(7+i,2,"Unlocking %s                          ",devref);
        move(7+i,22);
        if(d->MBREnabled()) {
            if(d->setMBRDone(1, password)) {
                LOG(E) << "Unlock failed - unable to set MBRDone";
                failed = 1;
            }
        }
        if  (d->setLockingRange(0, OPAL_LOCKINGSTATE::READWRITE, password)) {
            LOG(E) << "Unlock failed - unable to set LockingRange 0 RW";
            failed = 1;
        }
        printw("%s",(failed ? "Failed" : "Success"));
    delete d;             
    if (MAX_DISKS == i) {
        LOG(I) << MAX_DISKS << " disks, really?";
    }

}
    return 0x00;
};
