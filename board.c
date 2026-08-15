#include <stdio.h>
#include "types.h"

void setPropertyDetails(
                        BoardSquare *square,
                        PropertyGroup group,                //helper function
                        Money purchasePrice,
                        Money baseRent,
                        Money houseCost,
                        Money hotelCost,
                        Money mortageValue)

   {
      square->group = group;
      square->purchasePrice = purchasePrice;
      square->baseRent = baseRent;
      square->houseCost = houseCost;
      square->hotelCost = hotelCost;
      square->mortgageValue = mortageValue;


   }

void initializeBoard(Game *game)                         //board initialization      setPropertyLanding()
{
    const char *squareNames[BOARD_SIZE] = {
        "GO",
        "Pettah",
        "Community Development Fund",
        "Maradana",
        "Income Tax",
        "Colombo Fort Railway Station",
        "Bambalapitiya",
        "National Event Card",
        "Wellawatte",
        "Mount Lavinia",
        "Jail / Just Visiting",
        "Nugegoda",
        "Ceylon Electricity Board",
        "Maharagama",
        "Kottawa",
        "Kandy Railway Station",
        "Negombo",
        "Sri Lanka Insurance",
        "Katunayake",
        "Ja-Ela",
        "Free Parking",
        "Kandy City",
        "National Event Card",
        "Peradeniya",
        "Katugastota",
        "Galle Railway Station",
        "Galle Fort",
        "Unawatuna",
        "National Water Supply and Drainage Board",
        "Hikkaduwa",
        "Go To Jail",
        "Jaffna Town",
        "Nallur",
        "Ceylinco Insurance",
        "Trincomalee",
        "Jaffna Railway Station",
        "National Event Card",
        "Nuwara Eliya",
        "Bank of Ceylon",
        "Galle Face"
    };

    SquareType squareTypes[BOARD_SIZE] = {
        SQUARE_GO,
        SQUARE_PROPERTY,
        SQUARE_EVENT,
        SQUARE_PROPERTY,
        SQUARE_TAX,
        SQUARE_RAILWAY,
        SQUARE_PROPERTY,
        SQUARE_EVENT,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_JAIL,
        SQUARE_PROPERTY,
        SQUARE_UTILITY,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_RAILWAY,
        SQUARE_PROPERTY,
        SQUARE_INSURANCE,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_FREE_PARKING,
        SQUARE_PROPERTY,
        SQUARE_EVENT,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_RAILWAY,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_UTILITY,
        SQUARE_PROPERTY,
        SQUARE_GO_TO_JAIL,
        SQUARE_PROPERTY,
        SQUARE_PROPERTY,
        SQUARE_INSURANCE,
        SQUARE_PROPERTY,
        SQUARE_RAILWAY,
        SQUARE_EVENT,
        SQUARE_PROPERTY,
        SQUARE_BANK,
        SQUARE_PROPERTY
    };

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        square->index = i;
        square->name = squareNames[i];
        square->type = squareTypes[i];

        square->group = GROUP_NONE;

        square->purchasePrice = 0;
        square->baseRent = 0;

        square->houseCost = 0;
        square->hotelCost = 0;
        square->houses = 0;
        square->hasHotel = 0;

        square->mortgageValue = 0;
        square->mortgaged = 0;
        square->loanLocked = 0;

        square->currentMarketValue = 0;

        square->insuranceType = INSURANCE_NONE;

        square->insuranceExpiryRound = 0;
        square->insuranceReminderShown = 0;

        square->damaged = 0;
        square->repairCost = 0;

        square->closedUntilRound = 0;

        square->normalMarketValue = 0;
        square->normalBaseRent = 0;

        square->propertyAge = 0;
        square->depreciationPercent = 0;
        square->developmentDeadlineRound = 0;

        square->owner = NO_OWNER;
    }

                                                                                 /* Brown properties */
    setPropertyDetails(&game->board[1],
                       GROUP_BROWN, 1500, 100, 500, 2000, 750);

    setPropertyDetails(&game->board[3],
                       GROUP_BROWN, 1800, 120, 500, 2000, 750);

                                                                     /* Light Blue properties */
    setPropertyDetails(&game->board[6],
                       GROUP_LIGHT_BLUE, 2500, 180, 750, 3000, 1250);

    setPropertyDetails(&game->board[8],
                       GROUP_LIGHT_BLUE, 2700, 200, 750, 3000, 1250);

    setPropertyDetails(&game->board[9],
                       GROUP_LIGHT_BLUE, 3000, 220, 750, 3000, 1250);

                                                                        /* Pink properties */
    setPropertyDetails(&game->board[11],
                       GROUP_PINK, 3500, 260, 1000, 4000, 1750);

    setPropertyDetails(&game->board[13],
                       GROUP_PINK, 3800, 280, 1000, 4000, 1750);

    setPropertyDetails(&game->board[14],
                       GROUP_PINK, 4000, 300, 1000, 4000, 1750);

                                                                        /* Orange properties */
    setPropertyDetails(&game->board[16],
                       GROUP_ORANGE, 4500, 350, 1250, 5000, 2250);

    setPropertyDetails(&game->board[18],
                       GROUP_ORANGE, 4700, 370, 1250, 5000, 2250);

    setPropertyDetails(&game->board[19],
                       GROUP_ORANGE, 5000, 400, 1250, 5000, 2250);

                                                                        /* Red properties */
    setPropertyDetails(&game->board[21],
                       GROUP_RED, 5500, 450, 1500, 6000, 2750);

    setPropertyDetails(&game->board[23],
                       GROUP_RED, 5800, 480, 1500, 6000, 2750);

    setPropertyDetails(&game->board[24],
                       GROUP_RED, 6000, 500, 1500, 6000, 2750);

                                                                        /* Yellow properties */
    setPropertyDetails(&game->board[26],
                       GROUP_YELLOW, 6500, 600, 2000, 8000, 3250);

    setPropertyDetails(&game->board[27],
                       GROUP_YELLOW, 6800, 620, 2000, 8000, 3250);

    setPropertyDetails(&game->board[29],
                       GROUP_YELLOW, 7000, 650, 2000, 8000, 3250);

                                                                   /* Green properties */
    setPropertyDetails(&game->board[31],
                       GROUP_GREEN, 8000, 750, 2500, 10000, 4000);

    setPropertyDetails(&game->board[32],
                       GROUP_GREEN, 8300, 780, 2500, 10000, 4000);

    setPropertyDetails(&game->board[34],
                       GROUP_GREEN, 8500, 800, 2500, 10000, 4000);

                                                                   /* Dark Blue properties */
    setPropertyDetails(&game->board[37],
                       GROUP_DARK_BLUE, 10000, 1000, 3000, 12000, 5000);

    setPropertyDetails(&game->board[39],
                       GROUP_DARK_BLUE, 12000, 1200, 3000, 12000, 5000);

    game->board[5].purchasePrice = RAILWAY_PURCHASE_PRICE;
    game->board[15].purchasePrice = RAILWAY_PURCHASE_PRICE;
    game->board[25].purchasePrice = RAILWAY_PURCHASE_PRICE;
    game->board[35].purchasePrice = RAILWAY_PURCHASE_PRICE;
    
    game->board[5].mortgageValue = RAILWAY_MORTGAGE_VALUE;
    game->board[15].mortgageValue = RAILWAY_MORTGAGE_VALUE;
    game->board[25].mortgageValue = RAILWAY_MORTGAGE_VALUE;
    game->board[35].mortgageValue = RAILWAY_MORTGAGE_VALUE;

    game->board[12].purchasePrice = UTILITY_PURCHASE_PRICE;
    game->board[28].purchasePrice = UTILITY_PURCHASE_PRICE;

    game->board[12].mortgageValue = UTILITY_MORTGAGE_VALUE;
    game->board[28].mortgageValue = UTILITY_MORTGAGE_VALUE;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY ||
            square->type == SQUARE_RAILWAY ||
            square->type == SQUARE_UTILITY)
        {
            square->currentMarketValue = square->purchasePrice;
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY)
        {
            square->normalMarketValue = square->purchasePrice;
            square->currentMarketValue = square->purchasePrice;
            square->normalBaseRent = square->baseRent;
        }
    }
}


int movePlayer(Player *player, int diceTotal)              //player movement function

{
   int oldPosition = player->position;
   int nextPosition = oldPosition + diceTotal;
   int passedGo = nextPosition >= BOARD_SIZE;

   player->position = nextPosition % BOARD_SIZE;

   printf("%s moves from square %d to square %d.\n\n",
                  player->name,
                  oldPosition,
                  player->position);

   if ( passedGo == 1 ){

      player->cash = player->cash + GO_REWARD;

      printf("%s passed Go.\n", player->name);
      printf("Collected LKR %d.\n", GO_REWARD);
      printf("Current Balance : LKR %lld.\n\n", player->cash);
   }

   return passedGo;
}

void displayLanding(Game *game, Player *player)             //landed square details
{
    BoardSquare *square = &game->board[player->position];

    printf("%s landed on %s.\n", player->name, square->name);
    printf("Square Type : ");

    switch (square->type)
    {
        case SQUARE_GO:
        printf("GO\n");
        break;

        case SQUARE_PROPERTY:
        printf("Property\n");
        break;

        case SQUARE_EVENT:
        printf("Event\n");
        break;

        case SQUARE_TAX:
        printf("Tax\n");
        break;

        case SQUARE_RAILWAY:
        printf("Railway\n");
        break;

        case SQUARE_JAIL:
        printf("Jail / Just Visiting\n");
        break;

        case SQUARE_UTILITY:
        printf("Utility\n");
        break;

        case SQUARE_INSURANCE:
        printf("Insurance\n");
        break;

        case SQUARE_FREE_PARKING:
        printf("Free Parking\n");
        break;

        case SQUARE_GO_TO_JAIL:
        printf("Go to Jail\n");
        break;

        case SQUARE_BANK:
        printf("Bank\n");
        break;

        default:
        printf("Unknown\n");
        break;
    }

}

void sendPlayerToJail(Player *player)
{
    player->position = JAIL_POSITION;
    player->inJail = 1;
    player->jailTurns = 0;

    printf("%s has been sent to jail.\n", player->name);
    printf("New position : Square %d\n", player->position);

}