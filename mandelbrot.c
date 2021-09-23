#include <allegro5/allegro.h>
#include <stdio.h>
#include "f.h"

// use WASD to move around
// use UP/DOWN arrow to zoom in/out 
// use LEFT/RIGHT arrow to decrease/increase maximal iterations  (max 255/ min 20)
// use left mouse button to zoom in at selected  cooridnates
// use right mouse button to zoom out

// f - function drawing mandelbrot set
extern void f(unsigned char* pix_arr, int pixels, int iter, float x_left, float y_top, float step);

int main(int argc, char* argv[])
{   
    int MAX_IT = 80;                    // maximal number of iterations
    float RANGE = 4;                    // side of displayed area
    int pixels = 512;                   // height and width in pixels
    float x = 0;                        // initial x coordinate
    float y = 0;                        // initial y coordinate
    float step = RANGE/pixels;          // difference between coordinates in neighbour pixels
    float r = RANGE/2;                  // half of RANGE

    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue;
    if (!al_init()) 
    {
        fprintf(stderr, "Failed to initialize allegro.\n");
        return -1;
    }

    display = al_create_display(pixels, pixels);
    if (!display) 
    {
        fprintf(stderr, "Failed to create display.\n");
        return -1;  
    }
    
    ALLEGRO_BITMAP* bitmap = al_create_bitmap(pixels, pixels);
    if (!bitmap) 
    {
        fprintf(stderr, "Failed to create bitmap.\n");
        return -1;  
    }

    ALLEGRO_LOCKED_REGION *region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA, ALLEGRO_LOCK_READWRITE);
    if (!region) 
    {
        fprintf(stderr, "Failed to create locked region.\n");
        return -1;  
    }

    if(!al_install_keyboard())
    {
        fprintf(stderr, "Failed to install keyboard.\n");
        return 1;
    }

    if(!al_install_mouse())
    {
        fprintf(stderr, "Failed to install mous.\n");
        return 1;
    }

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    unsigned char* pixel_array = (unsigned char*) region->data; 
    f(pixel_array, pixels, MAX_IT, x-RANGE/2, y+RANGE/2, step);
    al_unlock_bitmap(bitmap);
    al_draw_bitmap(bitmap, 0, 0, 0);
    al_flip_display();

    while (true)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)          // exit programm
        {
            al_destroy_event_queue(event_queue);
            al_destroy_bitmap(bitmap);
            al_destroy_display(display);
            return 0;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)        // press RIGHT arrow to increase MAX_IT
            {
                MAX_IT += 5;
                if (MAX_IT > 255)
                {
                    MAX_IT = 255;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)    // press LEFT arrow to increase MAX_IT
            {
                MAX_IT -= 5;
                if (MAX_IT < 20)
                {
                    MAX_IT = 20;
                }
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_UP)           // press UP arrow to zoom in
            {
                RANGE = RANGE/2;
                step = step/2;
                r = RANGE/2;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)    // press DOWN arrow to zoom out
            {
                RANGE = RANGE*2;
                if (RANGE > 4)
                {
                    RANGE = 4;
                    x = 0;
                    y = 0;
                }
                r = RANGE/2;
                step = RANGE/pixels;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_W)       // press W to go up
            {
                y += (step*20);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_S)       // press S to go down
            {
                y -= (step*20);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_A)       // press A to go left
            {
                x -= (step*20);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_D)       // press D to go right
            {
                x += (step*20);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)  // press escape to exit
            {
                al_destroy_event_queue(event_queue);
                al_destroy_bitmap(bitmap);
                al_destroy_display(display);
                return 0;
            }
            
            region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA, ALLEGRO_LOCK_READWRITE);
            pixel_array = (unsigned char*) region->data; 
            f(pixel_array, pixels, MAX_IT, x-r, y+r, step);
            al_unlock_bitmap(bitmap);
            al_draw_bitmap(bitmap, 0, 0, 0);
            al_flip_display();

        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            ALLEGRO_MOUSE_STATE state;
            
            al_get_mouse_state(&state);
            
            if (state.buttons & 1)      // press left mouse button to zoom in at selected coordinates
            {   
                x += (event.mouse.x * step - RANGE/2);
                y -= (event.mouse.y *step - RANGE/2);
                RANGE = RANGE/2;
                step = step/2;
                r = RANGE/2;

                region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA, ALLEGRO_LOCK_READWRITE);
                pixel_array = (unsigned char*) region->data; 
                f(pixel_array, pixels, MAX_IT, x-r, y+r, step);
                al_unlock_bitmap(bitmap);
                al_draw_bitmap(bitmap, 0, 0, 0);
                al_flip_display();
            }
            else if (state.buttons & 2) // press right mouse button to zoom out
            {
                
                RANGE = RANGE*2;

                if (RANGE > 4)
                {
                    RANGE = 4;
                    x = 0;
                    y = 0;
                }
                r = RANGE/2;
                step = RANGE/pixels;

                region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA, ALLEGRO_LOCK_READWRITE);
                pixel_array = (unsigned char*) region->data; 
                f(pixel_array, pixels, MAX_IT, x-r, y+r, step);
                al_unlock_bitmap(bitmap);
                al_draw_bitmap(bitmap, 0, 0, 0);
                al_flip_display();
            }
        }
    }
}
