#include <stdio.h>
#include "types.h"

void initializePlayers(Game *game)                      //player initialization function
{
    const char *names[PLAYER_COUNT] = {
        "Aggressive Investor",
        "Conservative Banker",
        "Risk Taker",
        "Opportunistic Trader"
    };

    StrategyType strategies[PLAYER_COUNT] = {
        STRATEGY_AGGRESSIVE,
        STRATEGY_CONSERVATIVE,
        STRATEGY_RISK_TAKER,
        STRATEGY_OPPORTUNISTIC
    };

    for (int i = 0; i < PLAYER_COUNT; i++) {
        Player *player = &game->players[i];

        player->id = i;
        player->name = names[i];
        player->strategy = strategies[i];

        player->cash = STARTING_CASH;
        player->taxDue = 0;

        player->position = 0;
        player->bankrupt = 0;
        player->inJail = 0;
        player->jailTurns = 0;

        player->hasActiveLoan = 0;
        player->loanBalance = 0;
        player->loanInterestRate = 0;
        player->loanRoundsRemaining = 0;
        player->experiencedFinancialLoss =0;

        for (int card = 0; card < NATIONAL_EVENT_CARD_COUNT; card++)
        {
            player->nationalCardExpiry[card] = 0;
        }

        player->revaluationGroup = GROUP_NONE;

        game->turnOrder[i] = i;
    }
}


int shouldPayJailBail(Player *player)
{
    if (player->cash < JAIL_BAIL)
    {
        return 0;
    }

    Money remainingCash = player->cash - JAIL_BAIL;

    switch (player->strategy)
    {
        case STRATEGY_AGGRESSIVE:
            return remainingCash >= AGGRESIVE_RENT_RESERVE;

        case STRATEGY_CONSERVATIVE:
            return remainingCash >= STARTING_CASH / 2;

        case STRATEGY_RISK_TAKER:
            return 1;

        case STRATEGY_OPPORTUNISTIC:
            return remainingCash >= AGGRESIVE_RENT_RESERVE;

        default:
            return 0;
    }
}


int shouldBuySquare(Player *player, BoardSquare *square)
{
    if (player->cash < square->purchasePrice )
    {
        return 0;
    }

    Money remainingCash = player->cash - square->purchasePrice;

    switch (player->strategy)
    {
    case STRATEGY_AGGRESSIVE:
        return remainingCash >= AGGRESIVE_RENT_RESERVE;
        break;

    case STRATEGY_CONSERVATIVE:
        return remainingCash >= player->cash/2;
        break;
        
    case STRATEGY_RISK_TAKER:
        return 1;
        break;

    case STRATEGY_OPPORTUNISTIC:
        return 0;
        break;
    
    default:
        return 0;
        break;
    }
    
}

int purchaseSquare(Player *player, BoardSquare *square)
{
    if (square->owner != NO_OWNER)
    {
        return 0;
    }
    
    printf("\n%s is available for purchase.\n", square->name);
    printf("Purchase price: LKR %lld\n", square->purchasePrice);

    if (shouldBuySquare(player, square) == 0)
    {
        printf("%s declined to purchase %s.\n", player->name, square->name);

        return 0; 
    }

    player->cash = player->cash - square->purchasePrice;
    square->owner = player->id;

    printf("%s purchased %s for LKR %lld.\n", player->name, square->name, square->purchasePrice);
    printf("Remaining balance: LKR %lld\n", player->cash);

    return 1;   
} 

int shouldBidInAuction(Player *player, Money nextBid, Money marketValue)
{
    if (player->bankrupt == 1)
    {
        return 0;
    }

    if (nextBid > player->cash )
    {
        return 0;
    }
    
    switch (player->strategy)
    {

        case STRATEGY_AGGRESSIVE:
        {
            Money maximumBid = marketValue * AGGRESSIVE_AUCTION_LIMIT_PERCENT / 100;
            return nextBid <= maximumBid;
        }
 
        case STRATEGY_CONSERVATIVE:
        {
            return nextBid < marketValue;
        }

        case STRATEGY_RISK_TAKER:
        {
            return 1;
        }

        case STRATEGY_OPPORTUNISTIC:
        {
            Money maximumBid = marketValue * OPPORTUNISTIC_AUCTION_LIMIT_PERCENT / 100;
            return nextBid <= maximumBid;
        }

        default:
            return 0;
    }
    
}


int ownsMonopoly(Game *game, Player *player, PropertyGroup group)
{
    int totalProperties = 0;
    int ownedproperties = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY && square->group == group )
        {
            totalProperties++;

                    if (player->id == square->owner)
                    {
                        ownedproperties++;
                    }

        }
        

    }

    if (totalProperties > 0 && ownedproperties == totalProperties)
    {
        return 1;
    }
    
    return 0;
    
}

BoardSquare *findPropertyForNextHouse(Game *game, Player *player, PropertyGroup group)
{
    BoardSquare *selected = NULL;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY && 
            square->group == group &&
            player->id == square->owner &&
            square->hasHotel == 0 &&
            square->houses < 4)
        {
            if (selected == NULL || square->houses < selected->houses)
            {
                selected = square;
            }
            
        }
        
    }
    return selected;
    
}

BoardSquare *findPropertyForHotel(Game *game, Player *player, PropertyGroup group)
{
    BoardSquare *candidate = NULL;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY &&
            square->group == group &&
            square->owner == player->id)
        {
            if (square->hasHotel == 1)
            {
                continue;
            }

            if (square->houses != 4)
            {
                return  NULL;
            }

            if (candidate == NULL)
            {
                candidate = square;
            }  
        }    
    }
    return candidate; 
}

int shouldDevelopProperty(Game *game, Player *player, Money cost)
{

    if (isNationalEventCardActive(game, player, CARD_LABOUR_STRIKE))
{
    printf("%s cannot construct during the Labour Strike.\n", player->name);
    return 0;
}

    if (cost > player->cash)
    {
        return 0;
    }

    Money remainingCash = player->cash - cost;

    switch (player->strategy)
    {
    case STRATEGY_AGGRESSIVE:
        return 1;

    case STRATEGY_RISK_TAKER:
        return 1;

    case STRATEGY_CONSERVATIVE:

        return remainingCash >= player->cash /2;

    case STRATEGY_OPPORTUNISTIC:

            if (game->currentEconomicEvent == ECONOMIC_EVENT_HOUSING_PROGRAMME ||
                game->currentRegulation == REGULATION_HOUSING_SUBSIDY ||
                isNationalEventCardActive(game, player, CARD_HOUSING_SUBSIDY))
            {
                return 1;
            }

            if (game->currentInflationRate > 0)
            {
                return 0;
            }

            return 0;
    
    default:
        return 0;
    }
}

int developOneStep(Game *game, Player *player, PropertyGroup group)
{
    BoardSquare *hotelProperty = findPropertyForHotel(game, player, group);

    if (hotelProperty != NULL)
    {
        Money hotelCost = getCurrentHotelCost(game, hotelProperty);
        
        hotelCost = applyNationalConstructionCostModifiers(game, player, hotelCost, 0);        

        if (shouldDevelopProperty(game, player, hotelCost) == 0)
        {
            return 0;
        }
        
        player->cash = player->cash - hotelCost;

        hotelProperty->houses = 0;
        hotelProperty->hasHotel = 1;

        hotelProperty->developmentDeadlineRound = 0;

        printf("%s upgraded %s to a Hotel.\n", player->name, hotelProperty->name);
        printf("Hotel Cost: LKR %lld\n", hotelCost);
        printf("Remaining Balance: LKR %lld\n\n", player->cash);
    
        return 1;
    }

    BoardSquare *houseProperty = findPropertyForNextHouse(game, player, group);

    if (houseProperty == NULL)
    {
        return 0;
    }

    Money houseCost = getCurrentHouseCost(game, houseProperty);
    
    houseCost = applyNationalConstructionCostModifiers(game, player, houseCost, 1);

    if (shouldDevelopProperty(game, player, houseCost) == 0)
    {
        return 0;
    }
    
    player->cash = player->cash - houseCost;
    houseProperty->houses++;

    houseProperty->developmentDeadlineRound = 0;

    printf("%s constructed one house on %s.\n", player->name, houseProperty->name);
    printf("House Cost: LKR %lld\n", houseCost);
    printf("Houses on %s: %d\n", houseProperty->name, houseProperty->houses);
    printf("Remaining Balance: LKR %lld\n", player->cash);
    
    return 1;
}

void developProperties(Game *game, Player *player)
{
    PropertyGroup groups[8] = 
    {
        GROUP_BROWN,
        GROUP_LIGHT_BLUE,
        GROUP_PINK,
        GROUP_ORANGE,
        GROUP_RED,
        GROUP_YELLOW,
        GROUP_GREEN,
        GROUP_DARK_BLUE
    };

    for (int i = 0; i < 8; i++)
    {
        PropertyGroup group = groups[i];

        if (ownsMonopoly(game, player, group) == 0)
        {
            continue;
        }
        
        if (player->strategy == STRATEGY_AGGRESSIVE || player->strategy == STRATEGY_RISK_TAKER)
        {
            while (developOneStep(game, player, group) == 1)
            {
                //developing properties through developoneStep() function
            }
            
        }
        else
        {
            if (developOneStep(game, player, group) == 1)
            {
                return;
            } 
        }
    }  
}


int shouldRenovateProperty(Player *player, BoardSquare *square, Money renovationCost)
{
    if (square->depreciationPercent == 0)
    {
        return 0;
    }

    if (player->cash < renovationCost)
    {
        return 0;
    }

    switch (player->strategy)
    {
        case STRATEGY_AGGRESSIVE:
            return square->depreciationPercent >= AGGRESSIVE_RENOVATION_THRESHOLD;

        case STRATEGY_CONSERVATIVE:
            return square->depreciationPercent > 10;

        case STRATEGY_RISK_TAKER:
            return square->depreciationPercent >= RISK_TAKER_RENOVATION_THRESHOLD;

        case STRATEGY_OPPORTUNISTIC:
            return square->depreciationPercent > 15;

        default:
            return 0;
    }
}







