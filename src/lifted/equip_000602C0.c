/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);

int armor_image_for_type(int armor_type, int index)
{
    int image;

    switch (index) {
    case 0:
        if (armor_type == 0) {
            image = 3;
        } else if (armor_type == 1) {
            image = 7;
        } else {
            image = rand_range(4, 6);
        }
        break;
    case 1:
        if (armor_type == 0) {
            image = 8;
        } else {
            image = 9;
        }
        break;
    case 2:
        if (armor_type == 0) {
            image = rand_range(10, 11);
        } else if (armor_type == 1) {
            image = 16;
        } else {
            image = rand_range(12, 15);
        }
        break;
    case 3:
        if (armor_type == 0) {
            image = 17;
        } else if (armor_type == 1) {
            image = 21;
        } else {
            image = rand_range(18, 20);
        }
        break;
    case 4:
        if (armor_type == 0) {
            image = 22;
        } else if (armor_type == 1) {
            image = 26;
        } else {
            image = rand_range(23, 25);
        }
        break;
    case 6:
        if (armor_type == 0) {
            image = 0;
        } else {
            image = 1;
        }
        break;
    default:
        image = -1;
    }
    return image;
}
