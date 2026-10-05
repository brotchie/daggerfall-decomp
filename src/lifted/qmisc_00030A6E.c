/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char quest_faces[];
extern char quest_faces_quest[];
extern char quest_faces_object[];
extern char quest_faces_image[];
extern int quest_face_images[];
extern int D_00195D14;
extern struct quest *current_quest;

extern int rand_range(int, int);

void quest_face_add(struct record *object, int name_bank, int gender, int object_id)
{
    int slot;
    int face_index;
    int i;
    char *image;
    struct character *character;

    if (object->type == 18) {
        character = &object->data.character;
        gender = ((((int)(unsigned short)(character->flags & 1)) != 0) ? 1 : 0);
    } else if (gender != 0) {
        gender = 1;
    }
    slot = 0;
    while (*(int *)(quest_faces_object + (slot * 10)) != 0 && slot < 10) slot++;
    if (slot >= 10) return;
    *(int *)(quest_faces_object + (slot * 10)) = object_id;
    *(signed char *)(quest_faces_quest + (slot * 10)) = (signed char)current_quest->id;
    if (object->type != 18 && object->data.building.faction_id == 514) {
        face_index = gender + (name_bank * 2);
        quest_faces[slot * 10] = (((*(signed char *)&gender << 7) + (*(signed char *)&name_bank << 6)) + *(signed char *)&face_index) | 16;
        image = (char *)D_00195D14;
    } else {
        quest_faces[slot * 10] = ((*(signed char *)&gender << 7) + (*(signed char *)&name_bank << 6)) + rand_range(0, 9);
        image = (char *)quest_face_images[((gender * 2) + name_bank)];
    }
    i = 0;
    face_index = (int)(unsigned char)(quest_faces[slot * 10] & 15);
    while (i < face_index) {
        image = (((int)*(unsigned short *)(image + 10)) + image) + 12;
        i++;
    }
    *(char **)(quest_faces_image + (slot * 10)) = image;
}
