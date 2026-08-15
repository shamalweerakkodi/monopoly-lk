#include <stdio.h>
#include <stdlib.h>
#include "types.h"

DiceRoll rollDice(void)                             // rolldice() function
{
    DiceRoll roll;

    roll.die1 = (rand() % 6) +1;
    roll.die2 = (rand() % 6) +1;
    
    roll.total = roll.die1 + roll.die2;
    roll.isDouble = (roll.die1 == roll.die2);

    return roll;
}

void determineTurnOrder(Game *game)
{
    int contenders[PLAYER_COUNT];
    int contenderCount = PLAYER_COUNT;                  //contenders for first roll

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        contenders[i] = i;
    }

    printf("\nDetermining the first player\n");
    printf("-------------------------------\n");

    while (contenderCount > 1)
    {

        int rollTotals[PLAYER_COUNT] = {0};
        int highestTotal = -1;

        for (int i = 0; i < contenderCount; i++)
        {
            int playerID = contenders[i];
            DiceRoll roll = rollDice();

            rollTotals[i] = roll.total;

            printf("%s rolls %d.\n", game->players[playerID].name, roll.total);

            if ( roll.total > highestTotal )
            {
                highestTotal = roll.total;
            }    
        }

        int nextContenders[PLAYER_COUNT];                       //contenders for second roll
        int nextCount = 0;

        for (int i = 0; i < contenderCount; i++)
        {
            if (rollTotals[i] == highestTotal)
            {
                nextContenders[nextCount] = contenders[i];
                nextCount++; 
            }
            
            if (nextCount > 1)
            {
                printf("\nThere is a tie for the highest roll.\n");
                printf("The tied players roll again.\n\n");
            }
            
        }

        for (int i = 0; i < nextCount; i++)
        {
            contenders[i] = nextContenders[i];
        }

        contenderCount = nextCount;    
    }

    int startingPlayer = contenders[0];                             //find starting player

    for (int turn = 0; turn < PLAYER_COUNT; turn++)
    {
        game->turnOrder[turn] = (startingPlayer + turn) % PLAYER_COUNT;
    }

    printf("\n%s will begin the game.\n", game->players[startingPlayer].name);
    printf("\nTurn order : \n");

    for (int turn = 0; turn < PLAYER_COUNT; turn++)
    {
        int playerId = game->turnOrder[turn];

        printf("%d. %s\n", turn + 1, game->players[playerId].name);
    }
    
    printf("\n");
    

    
    
}

void resolveLanding(Game *game, Player *player, int diceTotal)                 // displayLanding()   handlePropertyLanding()    sendPlayerToJail()
{       

    BoardSquare *square = &game->board[player->position];

    displayLanding(game, player);

    switch (square->type)
    {
        case SQUARE_PROPERTY:
            handlePropertyLanding(game, player, square);
            break;

        case SQUARE_GO_TO_JAIL:
            sendPlayerToJail(player);
            break;

        case SQUARE_RAILWAY:
            handleRailwayLanding(game, player, square);
            break;

        case SQUARE_UTILITY:
            handleUtilityLanding(game, player, square, diceTotal);
            break;

        case SQUARE_JAIL:
            printf("%s is just visiting jail.\n", player->name);
            break;

        case SQUARE_FREE_PARKING:
            printf("Free parking : no action is required.\n");
            break;
        
        case SQUARE_TAX:
            handleIncomeTax(game, player);
            break;
        
        case SQUARE_EVENT:
            if (square->index == COMMUNITY_FUND_POSITION)
            {
                handleCommunityDevelopmentFund(game, player);
            }
            else
            {
                drawNationalEventCard(game, player);
            }
            break;

        case SQUARE_BANK:
            handleBankLanding(game, player);
            break;

        case SQUARE_INSURANCE:
            handleInsuranceLanding(game, player);
            break;

        case SQUARE_GO:
            printf("No additional action on GO.\n");
            break;
        
        default:
            break;
    }
}

int playBasicTurn(Game *game, Player *player)         //rollDice(), movePlayer(), resolveLanding()
{
    printf("==================================\n");
    printf("%s's Turn\n", player->name);
    printf("==================================\n");

    repairOwnedDamagedProperties(game, player);

    if (player->inJail == 1 && shouldPayJailBail(player) == 1)
    {
        player->cash -= JAIL_BAIL;

        player->inJail = 0;
        player->jailTurns = 0;

        printf("%s paid bail of LKR %d.\n", player->name, JAIL_BAIL);
        printf("%s has been released from jail.\n", player->name);
        printf("Remaining Balance: LKR %lld\n\n", player->cash);
    }

    DiceRoll roll = rollDice();

    printf("%s rolled %d.\n\n", player->name, roll.total);
    printf("Die 1 : %d\n", roll.die1);
    printf("Die 2 : %d\n", roll.die2);

    if (roll.isDouble == 1)
    {
            printf("The roll is a double.\n\n");
    }
    else
    {
            printf("The roll is not a double.\n\n");
    }

    if (player->inJail == 1)                            //checking whether player is in jail or not
    {
        printf("%s is currently in Jail.\n", player->name);

        if (roll.isDouble == 1)                             //rolls double
        {
            printf("%s rolls double and leaves the jail.\n", player->name);

            player->inJail = 0;
            player->jailTurns = 0;
        }
        else
        {
            player->jailTurns++;                        //jail turns
            
            printf("%s did not roll doubles.\n", player->name);
            printf("Jail turn %d of 3.\n", player->jailTurns);

            if (player->jailTurns >= 3)
            {
                player->inJail = 0;
                player->jailTurns = 0;

                printf("%s had completed 3 turns in jail.\n", player->name);
                printf("%s is now released from jail.\n", player->name);
            }
            else
            {
                printf("%s remains in jail.\n", player->name);
            }

            printf("Current Position : Square %d\n\n", player->position);

            return 0;
            
        }
        
    }
    
    int passedGo = movePlayer(player, roll.total);

    resolveLanding(game, player, roll.total);

        if (player->inJail == 0 && player->bankrupt == 0)
        {
            developProperties(game, player);
        }
    
    printf("Current Cash : %lld\n", player->cash);
    printf("Current Position : Square %d\n\n", player->position);

    return passedGo;

}

int findLastActivePlayer(Game *game)
{
    for (int turn = PLAYER_COUNT - 1; turn >= 0; turn--)
    {
        int playerId = game->turnOrder[turn];

        Player *player = &game->players[playerId];

        if (player->bankrupt == 0 && player->inJail == 0)
        {
            return playerId;
        }
        
    }

    return -1;
    
}

void runAuction(Game *game, BoardSquare *square)
{
    Money marketValue = square->currentMarketValue;

    Money openingBid = marketValue * AUCTION_START_PERCENT / 100;

    Money currentBid = openingBid;

    int highestBidder = NO_OWNER;

    int active[PLAYER_COUNT] = {0};
    int activeCount = 0;

    printf("\n==================================\n");
    printf("Auction Started\n");
    printf("==================================\n");

    printf("Square      : %s\n", square->name);

    printf("Market Value: LKR %lld\n", marketValue);

    printf("Opening Bid : LKR %lld\n\n", openingBid);

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        if (game->players[i].bankrupt == 0)
        {
            active[i] = 1;
            activeCount++;
        }
    }

    while (activeCount > 0)
    {
        for (int turn = 0; turn < PLAYER_COUNT; turn++)
        {
            int playerId = game->turnOrder[turn];

            Player *player = &game->players[playerId];

            if (active[playerId] == 0)
            {
                continue;
            }

            if (playerId == highestBidder)
            {
                continue;
            }

            Money nextBid = currentBid + AUCTION_INCREMENT;

            if (shouldBidInAuction(player, nextBid, marketValue) == 1)
            {
                currentBid = nextBid;

                highestBidder = playerId;

                printf("%s bids LKR %lld.\n", player->name, currentBid);
            }
            else
            {
                active[playerId] = 0;
                activeCount--;

                printf("%s withdraws.\n", player->name);
            }

            if (highestBidder != NO_OWNER && activeCount == 1)
            {
                break;
            }
        }

        if (highestBidder != NO_OWNER && activeCount == 1)
        {
            break;
        }

        if (activeCount == 0)
        {
            break;
        }
    }

    if (highestBidder == NO_OWNER)
    {
        printf("\nNo player placed a bid.\n");
        printf("%s remains with the Bank.\n", square->name);

        return;
    }

    Player *winner = &game->players[highestBidder];

    winner->cash = winner->cash - currentBid;

    square->owner = winner->id;

    applyAntiSpeculationDeadline(game, winner, square);

    printf("\n%s wins the auction.\n", winner->name);
    printf("Winning Bid      : LKR %lld\n", currentBid);
    printf("Remaining Balance: LKR %lld\n", winner->cash);
    printf("==================================\n\n");
}

int countSolventPlayers(Game *game)
{
    int count = 0;

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        if (game->players[i].bankrupt == 0)
        {
            count++;
        }
    }

    return count;
}

Player *selectWinner(Game *game)
{
    Player *winner = NULL;
    Money highestNetWorth = 0;

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        if (player->bankrupt == 1)
        {
            continue;
        }

        Money netWorth = calculatePlayerNetWorth(game, player);

        if (winner == NULL ||
            netWorth > highestNetWorth ||
            (netWorth == highestNetWorth && player->cash > winner->cash))
        {
            winner = player;
            highestNetWorth = netWorth;
        }
    }

    return winner;
}

void displayGameOver(Game *game)
{
    Player *winner = selectWinner(game);

    printf("\n=============================================\n");
    printf("GAME OVER\n");
    printf("=============================================\n");

    if (winner == NULL)
    {
        printf("No solvent player remains.\n");
        return;
    }

    Money assetValue = calculateTotalOwnedAssetValue(game, winner);
    Money buildingValue = calculateBuildingValue(game, winner);
    Money mortgageLiability = calculateMortgageLiability(game, winner);
    Money netWorth = calculatePlayerNetWorth(game, winner);

    printf("Winner               : %s\n", winner->name);
    printf("Total Cash           : LKR %lld\n", winner->cash);
    printf("Total Property Value : LKR %lld\n", assetValue);
    printf("Total Building Value : LKR %lld\n", buildingValue);
    printf("Outstanding Loan     : LKR %lld\n", winner->loanBalance);
    printf("Mortgage Liabilities : LKR %lld\n", mortgageLiability);
    printf("Taxes Due            : LKR %lld\n", winner->taxDue);
    printf("Net Worth            : LKR %lld\n", netWorth);

    printf("=============================================\n");
}

const char *strategyName(StrategyType strategy)
{
    switch (strategy)
    {
        case STRATEGY_AGGRESSIVE:

            return "Aggressive";

        case STRATEGY_CONSERVATIVE:

            return "Conservative";

        case STRATEGY_RISK_TAKER:

            return "Risk Taker";

        case STRATEGY_OPPORTUNISTIC:

            return "Opportunistic";

        default:

            return "Unknown Strategy";
    }
}

void displayPlayerRoundSummary(Game *game, Player *player)
{
    int propertyCount = 0;
    int railwayCount = 0;
    int utilityCount = 0;

    int houseCount = 0;
    int hotelCount = 0;

    int insuredCount = 0;
    int damagedCount = 0;
    int closedCount = 0;

    int mortgagedCount = 0;
    int loanLockedCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner != player->id)
        {
            continue;
        }

        if (square->type == SQUARE_PROPERTY)
        {
            propertyCount++;

            houseCount += square->houses;
            hotelCount += square->hasHotel;

        }
        else if (square->type == SQUARE_RAILWAY)
        {
            railwayCount++;
        }
        else if (square->type == SQUARE_UTILITY)
        {
            utilityCount++;
        }

        if (square->insuranceType != INSURANCE_NONE)
        {
            insuredCount++;
        }

        if (square->damaged == 1)
        {
            damagedCount++;
        }

        if (square->closedUntilRound >= game->currentRound)
        {
            closedCount++;
        }

        if (square->mortgaged == 1)
        {
            mortgagedCount++;
        }

        if (square->loanLocked == 1)
        {
            loanLockedCount++;
        }
    }

    Money assetValue = calculateTotalOwnedAssetValue(game, player);
    Money buildingValue = calculateBuildingValue(game, player);
    Money mortgageLiability = calculateMortgageLiability(game, player);
    Money netWorth = calculatePlayerNetWorth(game,player);

    printf("\n---------------------------------------------\n");
    printf("Player           : %s\n",player->name);
    printf("Strategy         : %s\n",strategyName(player->strategy));

    if (player->bankrupt == 1)
    {
        printf("Status           : BANKRUPT\n");
    }
    else
    {
        printf("Status           : SOLVENT\n");
    }

    printf("Position         : Square %d - %s\n",player->position,game->board[player->position].name);

    if (player->inJail == 1)
    {
        printf("Jail             : Yes - turn %d of 3\n",player->jailTurns);
    }
    else
    {
        printf("Jail             : No\n");
    }

    printf("\nFinancial Information\n");
    printf("---------------------\n");
    printf("Cash             : LKR %lld\n",player->cash);
    printf("Asset Value      : LKR %lld\n",assetValue);
    printf("Building Value   : LKR %lld\n",buildingValue);
    printf("Mortgage Debt    : LKR %lld\n",mortgageLiability);
    printf("Tax Due          : LKR %lld\n",player->taxDue);

    if (player->hasActiveLoan == 1)
    {
        int effectiveLoanRate = applyNationalLoanInterestModifiers(game,player,player->loanInterestRate);
        
        printf("Loan Balance     : LKR %lld\n",player->loanBalance);
        printf("Effective Rate   : %d%%\n",effectiveLoanRate);
        printf("Loan Rounds Left : %d\n",player->loanRoundsRemaining);
    }
    else
    {
        printf("Active Loan      : None\n");
    }

    printf("\nOwned Assets\n");
    printf("------------\n");
    printf("Properties       : %d\n",propertyCount);
    printf("Railways         : %d\n",railwayCount);
    printf("Utilities        : %d\n",utilityCount);
    printf("Houses           : %d\n",houseCount);
    printf("Hotels           : %d\n",hotelCount);
    printf("Insured Assets   : %d\n",insuredCount);
    printf("Damaged Assets   : %d\n",damagedCount);
    printf("Closed Assets    : %d\n", closedCount);
    printf("Mortgaged Assets : %d\n", mortgagedCount);
    printf("Loan-Locked      : %d\n", loanLockedCount);
    printf("\nNET WORTH        : LKR %lld\n",netWorth);
}

void displayRoundSummary(Game *game)
{
    int effectiveIncomeTaxRate = game->incomeTaxRate * game->governmentIncomeTaxMultiplier/ 100;

    printf("\n\n=============================================\n");
    printf("ROUND %d SUMMARY\n",game->currentRound);
    printf("=============================================\n");

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        displayPlayerRoundSummary(game,&game->players[i]);
    }

    printf("\n=============================================\n");
    printf("CURRENT ECONOMIC CONDITIONS\n");
    printf("=============================================\n");
    printf("Inflation Rate        : %+d%%\n",game->currentInflationRate);
    printf("Current Loan Rate     : %d%%\n",getEffectiveBankLoanInterestRate(game));
    printf("Income Tax Rate       : %d%%\n",effectiveIncomeTaxRate);
    printf("Community Fund Rate   : %d%%\n",game->communityFundTaxRate);
    printf("Economic Event        : %s\n",economicEventName(game->currentEconomicEvent));
    printf("Government Regulation : %s\n",governmentRegulationName(game->currentRegulation));
    printf("Solvent Players       : %d\n",countSolventPlayers(game));
    printf("=============================================\n");

    displayActiveNationalEventCards(game);
}



