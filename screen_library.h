#include "draw_print_library.h"

//List of functions
void ChangeCommand();                       //Interrupt function for registering change command
void ChangeScreen();                        //Changes the screen if needed.

void SetIntro();                            // Set Intro screen
void SetMain();                             // Set Main screen
void SetFirstAux();                         // Set First Auxiliary screen
void SetSecondAux();                        // Set Second Auxiliary screen

void MainUpdate();                          // Update information in Main screen
void FirstAuxUpdate();                      // Update information in First Auxiliary screen
void SecondAuxUpdate();                     // Update information in Second Auxiliary screen