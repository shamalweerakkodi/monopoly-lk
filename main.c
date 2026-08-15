#include <stdio.h>
#include <stdlib.h>                     //define types
#include <time.h>
#include "types.h"

int startMenu(void)
{
    int choice = -1;
    int character;

    while (1)
    {
        printf("\n========================\n");
        printf("1 - Start Game\n");
        printf("0 - End Program\n");
        printf("========================\n\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        if (choice == 1 || choice == 0)
        {
            return choice;
        }
        
       while ((character = getchar()) != '\n' &&
               character != EOF)
        {
            /* Clear the input buffer. */
        }

        printf("Invalid choice. Please enter only 1 or 0.\n");
    }
}

int main(void)
{

       srand((unsigned int)time(NULL));                //generate random number

       while (1)
       {

              Game game = {0};

              initializePlayers(&game);
              initializeBoard(&game);
              initializeFinance(&game);
              initializeEvents(&game);

              game.currentRound = 1;

              printf("\n----------------------\n");                                    //intro
              printf("MONOPOLY-LK Simulation\n");
              printf("----------------------\n\n");

              printf("Number of board squares : %d\n", BOARD_SIZE);
              printf("Number of players       : %d\n", PLAYER_COUNT);
              printf("Starting cash           : LKR %d\n", STARTING_CASH);
              printf("Current round           : %d\n\n", game.currentRound);


              printf("\n-------\n");
              printf("Players\n");                                                    //players intro
              printf("-------\n\n");

              for (int i = 0; i < PLAYER_COUNT; i++)
              {
                     printf("Player %d : %s\n", game.players[i].id + 1, game.players[i].name);
                     printf("Cash     : LKR %lld\n", game.players[i].cash);
                     printf("Position : Square %d\n\n", game.players[i].position);
              }

              if (startMenu() == 0)
              {
                     break;
              }
              
              determineTurnOrder(&game);                                        //player turn order

              int numberofrounds = MAX_ROUNDS;

              while (game.currentRound <= numberofrounds)
              {
                     printf("\n=================================\n");
                     printf("Round %d Begins\n", game.currentRound);
                     printf("=================================\n");


                     int roundFinished = 0;
                     
                     while (roundFinished == 0 && countSolventPlayers(&game) > 1)
                     {
                     for (int turn = 0; turn < PLAYER_COUNT; turn++)
                     {
                            int lastActivePlayer = findLastActivePlayer(&game);

                            int playerId = game.turnOrder[turn];

                            Player *currentPlayer = &game.players[playerId];

                            if (currentPlayer->bankrupt == 1)
                            {
                                   continue;
                            }              

                            int passedGo = playBasicTurn(&game, currentPlayer);
                            
                            checkAllPlayersSolvency(&game);

                            if (countSolventPlayers(&game) <= 1)
                            {
                                   roundFinished = 1;
                                   
                                   printf("\nRound %d ended because only one solvent player remains.\n", game.currentRound);
                                   displayRoundSummary(&game);
                                   break;
                            }

                            if (playerId == lastActivePlayer && passedGo == 1)
                            {
                                   roundFinished = 1;
                                   break;
                            }
                            
                     }
                     }

                     if (countSolventPlayers(&game) <= 1)
                     {
                            break;
                     }     

                     printf("=================================\n");
                     printf("Round %d Completed\n", game.currentRound);
                     printf("=================================\n");

                     processEndOfRoundLoans(&game);
                     processPropertyDepreciation(&game);
                     processInflationIfNeeded(&game);
                     processEconomicEventIfNeeded(&game);
                     processGovernmentRegulationIfNeeded(&game);
                     processAntiSpeculationDeadlines(&game);
                     processDisasterIfNeeded(&game);
                     processInsuranceEndOfRound(&game);
                     processNationalEventCardsEndOfRound(&game);


                     checkAllPlayersSolvency(&game);

                     displayRoundSummary(&game);

                     if (countSolventPlayers(&game) <= 1)
                     {
                            break;
                     }

                     game.currentRound++;
              }

              displayGameOver(&game);
              break;
 
       }

       printf("\n========================\n");
       printf("Program ended.\n");
       printf("========================\n");

       return 0;
}