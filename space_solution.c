#include "space_explorer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAXPLANETS 500

typedef struct{
    unsigned int *planet_history;
    int planet_history_index;
    unsigned int *visited_planets;
    int visited_planet_count;
    double shortest_distance;
    unsigned int shortest_distance_planet_id;
}ShipState;

ShipAction space_hop(unsigned int crt_planet,
                     unsigned int *connections,
                     int num_connections,
                     double distance_from_mixer,
                     void *ship_state) {
    ShipAction action;
    ShipState *state;

    // this runs only when ship_state is null
    if (ship_state == NULL) {
        state = malloc(sizeof(ShipState));
        state->shortest_distance = distance_from_mixer;
        state->shortest_distance_planet_id = crt_planet;
        state->planet_history = malloc(sizeof(unsigned int) * MAXPLANETS);
        state->visited_planet_count = 0;
        state->planet_history_index = 0;
        state->visited_planets = malloc(sizeof(unsigned int) * MAXPLANETS);
    } else {
        state = ship_state;
    }
    printf("Current planet: %u\n", crt_planet);
    printf("Distance from mixer: %.2f\n", distance_from_mixer);
    
    // first hop will be a random hop in hopes of getting closer to the mixer
    if(state->visited_planet_count == 0){
        state->visited_planets[state->visited_planet_count++] = crt_planet;
        state->planet_history[state->planet_history_index++] = crt_planet;
        action.next_planet = RAND_PLANET;
        printf("Travelling to: %u\n\n", action.next_planet);
        action.ship_state = state;
        return action;
    }

    // checks if crt planet has been visited, (no redundant entries)
    char visited = 0;
    for (int i = 0; i < state->visited_planet_count; i++) {
        if (state->visited_planets[i] == crt_planet) {
            visited = 1;
            break;
        }
    }
    // if not, added to visited planets array
    if (visited == 0) {
        state->visited_planets[state->visited_planet_count++] = crt_planet;
        printf("Added planet: %u\n", state->visited_planets[state->visited_planet_count - 1]);
    }
    printf("Visited planets: %d\n", state->visited_planet_count);

    // this counts the numebr of unvisited planets within the connections
    int not_visited_planets = 0;
    for (int i = 0; i < num_connections; i++) {
        visited = 0; // reset visited to 0 on each iteration
        for (int j = 0; j < state->visited_planet_count; j++) {
            if (state->visited_planets[j] == connections[i]) {
                visited = 1;
                break;
            }
        }
        if (!visited) {
            not_visited_planets++;
        }
    }

    // this checks if the crt planet distance is closer than previous
    if (distance_from_mixer <= state->shortest_distance) {
        //if statement true therefore shortest distance is updated
        state->shortest_distance = distance_from_mixer;
        state->shortest_distance_planet_id = crt_planet;
        // this checks for number of unvisited planets from the connections
        if (not_visited_planets > 0) {
            // current plaent is added to the history array
            state->planet_history[state->planet_history_index++] = crt_planet;
            // this again checks if the connections have already been visisted
            for (int i = 0; i < num_connections; i++) {
                int connection_visited = 0;
                for (int j = 0; j < state->visited_planet_count; j++) {
                    if (connections[i] == state->visited_planets[j]) {
                        connection_visited = 1;
                        break;
                    }
                }
                // this is where it selects the next planet within connections
                if (!connection_visited) {
                    action.next_planet = connections[i];
                    break;
                }
            }
            // otherwise, the crt planet is added to planet history array and a random planet is chosen (no connections
            // have been not visited)
        } else {
            state->planet_history[state->planet_history_index++] = crt_planet;
            action.next_planet = RAND_PLANET;
        }
        // this is used to backtrack, if the crt planet is furhter than the shortest distance, you return to previous planet
    } else {
        // however first, planet history index is checked if it is not 0, if it is, you have to random hop (no planets to
        // backtrack to)
        if (state->planet_history_index > 0) {
            action.next_planet = state->planet_history[--state->planet_history_index];
            printf("backtracking...\n");
        } else {
                action.next_planet = RAND_PLANET;
        }
    }

    printf("Travelling to: %u\n\n", action.next_planet);

    action.ship_state = state;
    return action;
}