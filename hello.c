/*
 * hello.c
 * 2023-09-26 pc31754
 *
 * Write "Hello, World!" to the screen, and also mess with the controller.
 *
 */


#include <kos.h>
#include <stdbool.h>

KOS_INIT_FLAGS(INIT_DEFAULT);


// For some reason the program segfaults if I use a romdisk, so I'll have to figure out later how to fix that.
//extern uint8 romdisk[];
//KOS_INIT_ROMDISK(romdisk);


// Global variables
bool verbose = true;

struct rgb {
	float r;
	float g;
	float b;
};

struct rgb background_color;


void set_background_color(struct rgb color) {
	background_color = color;
	pvr_set_bg_color(color.r, color.g, color.b);
}


void update() {
	printf("Updating the screen\n");
	// Basic pvr render loop
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);
	pvr_list_finish();
	pvr_scene_finish();
}


// Initialize controller, pvr defaults and background color
void initialize() {
	printf("Initializing controller, pvr defaults and gray background color\n");
	
	struct rgb init_background_color;
	init_background_color.r = 0.50f;
	init_background_color.g = 0.50f;
	init_background_color.b = 0.50f;

	/* Surely there's a better way to detect a controller?		
	 /  This is my first time programming in C, so I want to see if I could just do something like
	 /  if(maple_enum_type(0, MAPLE_FUNC_CONTROLLER) printf("Controller found!\n");
	*/
	maple_device_t *device = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
	if(device) printf("Controller found!\n");
	else {
		printf("Controller not found!\nPlease plug in your controller and reboot the console.\n");
		exit_program = true;
		return;
	}

	pvr_init_defaults();
	set_background_color(init_background_color);
	update();
}


// Returns the passed controller's state. If there is no state, return NULL.
// I don't know if there's a controller state that returns null... I'll have to investigate.
cont_state_t * get_state(maple_device_t * controller) {
	if(controller) {
		cont_state_t * controller_status = (cont_state_t *)(maple_dev_status(controller));
		return controller_status;
	}
	return NULL;
}


bool state_pressed(cont_state_t * state) {
	if(state->buttons) {
		return 1;
	}
	return 0;
}


int main(int argc, char * iargv[]) {
	initialize();
	maple_device_t * controller = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);	

	while(1) {
		cont_state_t * controller_state = get_state(controller);

		// TODO: handle a bunch of other buttons too
		if(state_pressed(controller_state)) {
			if(verbose) {
				printf("Controller state:\n"
					"a: %d\n"
					"b: %d\n"
					"y: %d\n"
					"start: %d\n",
					"hjkl: %d%d%d%d\n",
					controller_state->a,
					controller_state->b,
					controller_state->y,
					controller_state->start,
					controller_state->dpad_left,
					controller_state->dpad_down,
					controller_state->dpad_up,
					controller_state->dpad_right);
			}
			if(controller_state->buttons & CONT_START) {
				printf("You pressed the start button! Exiting the program.\n");
				return 0;
			}
			update();
		}
	}
}
