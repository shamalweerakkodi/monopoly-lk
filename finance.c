#include <stdio.h>
#include "types.h"

void initializeFinance(Game *game)
{
    game->incomeTaxRate = INCOME_TAX_BASE_RATE;
    game->communityFundTaxRate = COMMUNITY_FUND_BASE_RATE;
    game->currentLoanInterestRate = STABLE_LOAN_INTEREST_RATE;
    game->currentInflationRate = 0;
}




int isOwnableSquare(BoardSquare *square)
{
    return square->type == SQUARE_PROPERTY ||
           square->type == SQUARE_RAILWAY ||
           square->type == SQUARE_UTILITY;
}

Money calculateTotalOwnedAssetValue(Game *game, Player *player)
{
    Money totalValue = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && isOwnableSquare(square))
        {
            totalValue += calculateNationalAdjustedAssetValue(game, player, square);
        }
    }

    return totalValue;
}

Money calculateBuildingValue(Game *game, Player *player)
{
    Money totalBuildingValue = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY ||
            square->owner != player->id)
        {
            continue;
        }

        if (square->hasHotel == 1)
        {
            
            totalBuildingValue += (4 * square->houseCost) + square->hotelCost;
        }
        else
        {
            totalBuildingValue += square->houses * square->houseCost;
        }
    }

    return totalBuildingValue;
}

Money calculateMortgageLiability(Game *game, Player *player)
{
    Money totalLiability = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id &&
            square->mortgaged == 1)
        {
            totalLiability += square->mortgageValue;
        }
    }

    return totalLiability;
}

Money calculatePlayerNetWorth(Game *game,Player *player)
{
    Money netWorth = player->cash;

    netWorth += calculateTotalOwnedAssetValue(game, player);

    netWorth += calculateBuildingValue(game, player);

    netWorth -= player->loanBalance;
    netWorth -= player->taxDue;

    netWorth -= calculateMortgageLiability(game, player);

    return netWorth;
}

void checkAllPlayersSolvency(Game *game)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        if (player->bankrupt == 1)
        {
            continue;
        }

        Money netWorth = calculatePlayerNetWorth(game, player);

        if (netWorth < 0)
        {
            printf("\n%s is insolvent.\n", player->name);
            printf("Net Worth : LKR %lld\n", netWorth);

            declareBankruptcy(game, player);
        }
    }
}

void resetAssetForBank(BoardSquare *square)
{
    square->owner = NO_OWNER;

    square->houses = 0;
    square->hasHotel = 0;

    square->mortgaged = 0;
    square->loanLocked = 0;

    square->insuranceType = INSURANCE_NONE;
    square->insuranceExpiryRound = 0;
    square->insuranceReminderShown = 0;

    square->damaged = 0;
    square->repairCost = 0;

    square->developmentDeadlineRound = 0;
}

int sellOneBuildingForDebt(Game *game, Player *player)
{
    BoardSquare *selected = NULL;
    Money bestRefund = 0;
    int sellingHotel = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY || square->owner != player->id)
        {
            continue;
        }

        Money refund = 0;
        int isHotel = 0;

        if (square->hasHotel == 1)
        {
            refund = getCurrentHotelCost(game, square) * BUILDING_SELL_PERCENT / 100;

            isHotel = 1;
        }
        else if (square->houses > 0)
        {
            refund = getCurrentHouseCost(game, square) * BUILDING_SELL_PERCENT / 100;
        }

        if (refund > bestRefund)
        {
            selected = square;
            bestRefund = refund;
            sellingHotel = isHotel;
        }
    }

    if (selected == NULL)
    {
        return 0;
    }

    if (sellingHotel == 1)
    {
        selected->hasHotel = 0;

        if (selected->insuranceType == INSURANCE_BUSINESS_INTERRUPTION)
        {
            selected->insuranceType = INSURANCE_NONE;

            selected->insuranceExpiryRound = 0;
            selected->insuranceReminderShown = 0;
        }

        printf("Hotel on %s sold to the Bank.\n", selected->name);
    }
    else
    {
        selected->houses--;
        printf("One house on %s sold to the Bank.\n", selected->name);
    }

    player->cash += bestRefund;

    printf("Building Refund : LKR %lld\n", bestRefund);
    printf("Current Cash    : LKR %lld\n", player->cash);

    return 1;
}

BoardSquare *findMortgageCandidate(Game *game, Player *player)
{
    BoardSquare *selected = NULL;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner != player->id ||
            isOwnableSquare(square) == 0 ||
            square->mortgaged == 1 ||
            square->loanLocked == 1 ||
            square->houses > 0 ||
            square->hasHotel == 1 ||
            square->mortgageValue <= 0)
        {
            continue;
        }

        if (selected == NULL ||
            square->mortgageValue > selected->mortgageValue)
        {
            selected = square;
        }
    }

    return selected;
}

int mortgageOneAssetForDebt(Game *game, Player *player)
{
    BoardSquare *square = findMortgageCandidate(game, player);

    if (square == NULL)
    {
        return 0;
    }

    square->mortgaged = 1;

    player->cash += square->mortgageValue;

    printf("%s mortgaged %s.\n", player->name, square->name);
    printf("Mortgage Amount : LKR %lld\n", square->mortgageValue);
    printf("Current Cash    : LKR %lld\n", player->cash);

    return 1;
}

BoardSquare *findAssetSaleCandidate(Game *game, Player *player)
{
    BoardSquare *selected = NULL;
    Money lowestValue = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner != player->id ||
            isOwnableSquare(square) == 0 ||
            square->loanLocked == 1 ||
            square->houses > 0 ||
            square->hasHotel == 1)
        {
            continue;
        }

        Money saleValue = calculateNationalAdjustedAssetValue(game, player, square);

        if (square->mortgaged == 1)
        {
            saleValue -= square->mortgageValue;
        }

        if (saleValue <= 0)
        {
            continue;
        }

        if (selected == NULL || saleValue < lowestValue)
        {
            selected = square;
            lowestValue = saleValue;
        }
    }

    return selected;
}

int sellOneAssetForDebt(Game *game, Player *player)
{
    BoardSquare *square = findAssetSaleCandidate(game, player);

    if (square == NULL)
    {
        return 0;
    }

    Money saleValue = calculateNationalAdjustedAssetValue(game, player, square);

    if (square->mortgaged == 1)
    {
        saleValue -= square->mortgageValue;
    }

    printf("%s sold %s to the Bank.\n", player->name, square->name);

    printf("Sale Value      : LKR %lld\n", saleValue);

    player->cash += saleValue;

    resetAssetForBank(square);

    printf("Current Cash    : LKR %lld\n", player->cash);

    return 1;
}

void declareBankruptcy(Game *game, Player *player)
{
    if (player->bankrupt == 1)
    {
        return;
    }

    player->bankrupt = 1;

    printf("\n==================================\n");
    printf("BANKRUPTCY\n");
    printf("==================================\n");

    printf("%s has been declared bankrupt.\n", player->name);

    printf("All buildings have been removed.\n");
    printf("All insurance policies have expired.\n");

    if (player->hasActiveLoan == 1)
    {
        printf("Outstanding Loan Immediately Due: LKR %lld\n", player->loanBalance);
    }

    printf("Remaining assets transferred to the Bank.\n");

    int auctionSquares[BOARD_SIZE];
    int auctionCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && isOwnableSquare(square))
        {
            printf("Transferred: %s\n", square->name);

            resetAssetForBank(square);

            auctionSquares[auctionCount] = i;
            auctionCount++;
        }
    }

    player->cash = 0;
    player->taxDue = 0;

    player->hasActiveLoan = 0;
    player->loanBalance = 0;
    player->loanInterestRate = 0;
    player->loanRoundsRemaining = 0;

    player->inJail = 0;
    player->jailTurns = 0;

    for (int i = 0; i < auctionCount; i++)
    {
        runAuction(game, &game->board[auctionSquares[i]]);
    }
}

int recoverDebt(Game *game, Player *debtor, Player *creditor, Money amount, const char *reason)
{
    if (amount <= 0)
    {
        return 1;
    }

    if (debtor->bankrupt == 1)
    {
        return 0;
    }

    printf("\n--- Debt Recovery ---\n");
    printf("Debtor          : %s\n", debtor->name);
    printf("Reason          : %s\n", reason);
    printf("Amount Required : LKR %lld\n", amount);

    Money remainingDebt = amount;
    Money totalRecovered = 0;

    while (remainingDebt > 0)
    {
        Money cashPayment = debtor->cash;

        if (cashPayment > remainingDebt)
        {
            cashPayment = remainingDebt;
        }

        debtor->cash -= cashPayment;
        remainingDebt -= cashPayment;
        totalRecovered += cashPayment;

        if (remainingDebt == 0)
        {
            break;
        }

        if (sellOneBuildingForDebt(game, debtor) == 1)
        {
            continue;
        }

        if (mortgageOneAssetForDebt(game, debtor) == 1)
        {
            continue;
        }

        if (sellOneAssetForDebt(game, debtor) == 1)
        {
            continue;
        }

        break;
    }

    if (creditor != NULL)
    {
        creditor->cash += totalRecovered;
    }

    printf("Amount Recovered: LKR %lld\n", totalRecovered);

    if (remainingDebt == 0)
    {
        printf("Debt paid in full.\n");
        printf("Remaining Cash  : LKR %lld\n", debtor->cash);

        if (creditor != NULL)
        {
            printf("Creditor        : %s\n", creditor->name);
            printf("Creditor Cash   : LKR %lld\n", creditor->cash);
        }

        return 1;
    }

    printf("Unpaid Debt     : LKR %lld\n", remainingDebt);
    printf("The player's available assets cannot cover the debt.\n");

    declareBankruptcy(game, debtor);

    return 0;
}



Money calculateOwnedPropertyValue(Game *game, Player *player)
{
    Money totalValue = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY && square->owner == player->id)
        {
            totalValue = totalValue + calculateNationalAdjustedAssetValue(game, player, square);
        }
            
    }

    return totalValue;
    
}

Money calculateIncomeTax(Game *game, Player *player)
{
    Money tax = player->cash * game->incomeTaxRate/100;
    
    tax = tax * game->governmentIncomeTaxMultiplier /100;
    
    return tax;

}

Money calculateCommunityFundTax(Game *game, Player *player)
{
    Money propertyValue = calculateOwnedPropertyValue(game, player);

    Money totalAssets = player->cash + propertyValue;

    Money tax = totalAssets * game->communityFundTaxRate/100;

    return tax;
}

void payTax(Game *game, Player *player, Money taxAmount, const char *taxName)
{
    if (taxAmount <= 0)
    {
        printf("No tax payment is required.\n");
        return;
    }

    printf("%s due: LKR %lld\n", taxName, taxAmount);

    player->taxDue += taxAmount;

    if (recoverDebt(game, player, NULL, player->taxDue, taxName) == 1)
    {
        player->taxDue = 0;
    }
}

void handleIncomeTax(Game *game, Player *player)
{
    printf("\n--- Income Tax ---\n");
    
    int effectiveIncomeTaxRate = game->incomeTaxRate * game->governmentIncomeTaxMultiplier / 100;
    
    printf("Current Income Tax Rate: %d%%\n", effectiveIncomeTaxRate);
    printf("Taxable Cash: LKR %lld\n", player->cash);

    Money tax = calculateIncomeTax(game, player);

    payTax(game, player, tax, "Income Tax");
}

void handleCommunityDevelopmentFund(Game *game, Player *player)
{
    printf("\n--- Community Development Fund ---\n");

    Money propertyValue = calculateOwnedPropertyValue(game, player);

    Money totalAssets = player->cash + propertyValue;

    printf("Current Fund Tax Rate : %d%%\n", game->communityFundTaxRate);
    printf("Cash                  : LKR %lld\n", player->cash);
    printf("Property Value        : LKR %lld\n", propertyValue);
    printf("Total Assets          : LKR %lld\n", totalAssets);

    Money tax = calculateCommunityFundTax(game, player);

    payTax(game, player, tax, "Community Development Fund Tax");
}



Money calculatePropertyRent(BoardSquare *square)
{
    int multiplier = 1;

    if (square->hasHotel == 1)
    {
        multiplier = 10;
    }
    else
    {
        switch (square->houses)
        {
        case 0:
            multiplier = 1;
            break;

        case 1:
            multiplier = 2;
            break;

        case 2:
            multiplier = 3;
            break;

        case 3:
            multiplier = 5;
            break;

        case 4:
            multiplier = 7;
            break;
        
        default:
            multiplier = 1;
            break;
        }
    }
    
    Money rent = square->baseRent * multiplier;
    return rent;
}

void payPropertyRent(Game *game, Player *visitor, BoardSquare *square)                      //pay the rent
{

    if (square->mortgaged == 1)
    {
        printf("%s is mortgaged. No rent is collected.\n", square->name);
        return;
    }

    if (square->closedUntilRound >= game->currentRound)
    {
        printf("%s is temporarily closed.\n", square->name);
        printf("No rent is collected.\n");
        return;
    }

    if (square->damaged == 1)
    {
        printf("%s is currently damaged.\n",square->name);
        printf("No rent can be collected until repairs are completed.\n");
        return;
    }

    int ownerId = square->owner;

    Player *owner = &game->players[ownerId];
    Money rent = calculatePropertyRent(square);

    if (square->hasHotel == 1)
    {
        rent = rent * game->eventHotelRentMultiplier/ 100;

        if (isNationalEventCardActive(game, owner, CARD_TOURISM_HYPE))
        {
            rent = rent * TOURISM_HYPE_RENT_MULTIPLIER/ 100;
        }

        if (isNationalEventCardActive(game, owner, CARD_FESTIVAL_SEASON))
        {
            rent = rent * FESTIVAL_SEASON_RENT_MULTIPLIER / 100;
        }
            printf("Development: Hotel\n");
    }
    else
    {
        printf("Houses: %d\n", square->houses);
    }
    printf("Rent for %s is LKR %lld.\n", square->name, rent);

    recoverDebt(game, visitor, owner, rent, "Property Rent");
}

void handlePropertyLanding(Game *game, Player *player, BoardSquare *square)           //payPropertyRent
{
    if (square->owner == NO_OWNER)
    {
        int purchased = purchaseSquare(player, square);

        if (purchased == 1)
        {
            applyAntiSpeculationDeadline(game, player, square);
        }
        else
        {
            runAuction(game, square);
        }

        return;
    }

    if (square->owner == player->id)
    {
        printf("\n%s already owns %s.\n", player->name, square->name);
        renovateProperty(player, square);
        return;
    }

    Player *owner = &game->players[square->owner];

    printf("\n%s is owned by %s\n", square->name, owner->name);
        
    payPropertyRent(game, player, square);
}



int countOwnedRailways(Game *game, int ownerId)
{
    int count = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (game->board[i].type == SQUARE_RAILWAY && game->board[i].owner == ownerId)
        {
            count++;
        }
    }

    return count;
}

Money calculateRailwayRent(int railwayCount)
{
    switch (railwayCount)
    {
        case 1:
            return 250;

        case 2:
            return 500;

        case 3:
            return 1000;

        case 4:
            return 2000;

        default:
            return 0;
    }
}

void handleRailwayLanding(Game *game, Player *visitor, BoardSquare *square)
{
    if (square->owner == NO_OWNER)
    {
        int purchased = purchaseSquare(visitor, square);

        if (purchased == 0)
        {
            runAuction(game, square);
        }

        return;
    }

    if (square->owner == visitor->id)                               //player is owner
    {
        printf("%s already owns %s.\n", visitor->name, square->name);           
        printf("No rent will be paid.\n");

        return;
    }

    if (square->mortgaged == 1)
    {
        printf("%s is mortgaged. No rent is collected.\n", square->name);
        return;
    }

    int ownerId = square->owner;                                   //pay rent
    Player *owner = &game->players[ownerId];

    int railwayCount = countOwnedRailways(game, ownerId);

    Money rent = calculateRailwayRent(railwayCount);
          rent = rent * game->eventRailwayRentMultiplier /100;
          rent = rent * game->governmentRailwayRentMultiplier /100;
          
          if (isNationalEventCardActive(game, owner, CARD_FUEL_SHORTAGE))
            {
                rent = rent * FUEL_SHORTAGE_RENT_MULTIPLIER/ 100;
            }

    printf("%s owns %d railway station(s).\n", owner->name, railwayCount);
    printf("Railway rent: LKR %lld\n", rent);

    recoverDebt(game, visitor, owner, rent, "Railway Rent");
}



int countOwnedUtilities(Game *game, int ownerId)
{
    int count = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (game->board[i].type == SQUARE_UTILITY && game->board[i].owner == ownerId)
        {
            count++;
        }
    }

    return count;
}

Money calculateUtilityRent(int utilityCount, int diceTotal)
{
    switch (utilityCount)
    {
        case 1:
            return 4*diceTotal;
            break;

        case 2:
            return 10*diceTotal;
            break;

        default:
            return 0;
    }
}

void handleUtilityLanding(Game *game, Player *visitor, BoardSquare *square, int diceTotal)
{
    if (square->owner == NO_OWNER)                                                                  //no owner - purchase
    {
        int purchased = purchaseSquare(visitor, square);

        if (purchased == 0)
        {
            runAuction(game, square);
        }

        return;
    }

    if (square->owner == visitor->id)                                                               //player is owner
    {
        printf("%s already owns %s.\n", visitor->name, square->name);
        printf("No rent will be paid.\n");

        return;
    }

    if (square->mortgaged == 1)
    {
        printf("%s is mortgaged. No rent is collected.\n", square->name);
        return;
    }

    int ownerId = square->owner;                                                                      //pay rent
    Player *owner = &game->players[ownerId];

    int utilityCount = countOwnedUtilities(game, ownerId);

    Money rent = calculateUtilityRent(utilityCount, diceTotal);
          rent = rent * game->governmentUtilityRentMultiplier /100;

    if (isNationalEventCardActive(game, owner, CARD_POWER_FAILURE))
    {
        rent = rent * POWER_FAILURE_RENT_MULTIPLIER/ 100;
    }

    printf("%s owns %d utility compay(s).\n", owner->name, utilityCount);
    printf("\nDice Total: %d\n", diceTotal);
    printf("Utility rent: LKR %lld\n", rent);

    recoverDebt(game, visitor, owner, rent, "Utility Rent");
}



Money calculateCollateralValue(Game *game, Player *player)
{
    Money totalValue = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];
       
        if (square->mortgaged == 0 && square->owner == player->id && 
            (square->type == SQUARE_PROPERTY || square->type == SQUARE_RAILWAY || square->type == SQUARE_UTILITY))
        {
            totalValue = totalValue + square->mortgageValue;
        }     
    }
    return totalValue;
}

Money calculateMaximumLoan(Game *game, Player *player)
{
    Money totalValue = calculateCollateralValue(game, player);

    Money maximumLoan = totalValue * MAX_LOAN_PERCENT/100;

    return maximumLoan;
}

void lockEligibleCollateral(Game *game, Player *player)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->mortgaged == 0 && square->owner == player->id && 
            (square->type == SQUARE_PROPERTY || square->type == SQUARE_RAILWAY || square->type == SQUARE_UTILITY))
        {
            square->loanLocked = 1;
        }


    }
}

void displayLockedCollateral(Game *game, Player *player)
{
    printf("Collateral:\n");

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && square->loanLocked == 1)
        {
            printf("- %s\n", square->name);
        }
    }
}

int obtainLoan(Game *game, Player *player, Money requestedAmount)
{
    if (player->hasActiveLoan == 1)
    {
        printf("%s already has an active loan.\n", player->name);
        return 0;
    }

    Money maximumLoan = calculateMaximumLoan(game, player);

    if (maximumLoan <= 0)
    {
        printf("%s has no eligible collateral.\n", player->name);        
        return 0;
    }

    if (requestedAmount > maximumLoan)
    {
        requestedAmount = maximumLoan;
    }

    if (requestedAmount <= 0)
    {
        return 0;
    }

    lockEligibleCollateral(game, player);

    player->hasActiveLoan = 1;
    player->loanBalance = requestedAmount;
    player->loanInterestRate = getEffectiveBankLoanInterestRate(game);
    player->loanRoundsRemaining = LOAN_DURATION;
    player->cash = player->cash + requestedAmount;
    
    printf("\n%s obtained a secured loan.\n", player->name);
    printf("Loan Amount     : LKR %lld\n", requestedAmount);
    displayLockedCollateral(game, player);
    printf("Interest Rate   : %d%%\n", player->loanInterestRate);
    printf("Duration        : %d rounds\n", player->loanRoundsRemaining);
    printf("Current Balance : LKR %lld\n", player->cash);

    return 1;
}

void unlockloanCollateral(Game *game, Player *player)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id)
        {
            square->loanLocked = 0;
        }   
    }
}

void repayLoan(Game *game, Player *player, Money loanAmount)
{
    if (player->hasActiveLoan == 0)
    {
        printf("%s has no active loan.\n", player->name);
        return;
    }

    if (loanAmount <= 0)
    {
        return;
    }
    
    if (loanAmount > player->cash)
    {
        loanAmount = player->cash;
    }

    if (loanAmount > player->loanBalance)
    {
        loanAmount = player->loanBalance;
    }

    player->cash = player->cash - loanAmount;
    player->loanBalance = player->loanBalance - loanAmount;

    printf("%s repaid LKR %lld.\n", player->name, loanAmount);
    printf("Outstanding Loan: LKR %lld\n", player->loanBalance);

    if (player->loanBalance == 0)
    {
        player->hasActiveLoan = 0;
        player->loanInterestRate = 0;
        player->loanRoundsRemaining = 0;

        unlockloanCollateral(game, player);

        printf("Loan has been fully repaid.\n");
        printf("Collateral has been released.\n");
    }
}

int increaseLoanToMaximum(Game *game, Player *player)
{
    if (player->hasActiveLoan == 0)
    {
        return 0;
    }

    Money maximumLoan = calculateMaximumLoan(game, player);

    if (maximumLoan <= player->loanBalance)
    {
        printf("No additional loan amount is available.\n");
        return 0;
    }

    Money additionalAmount = maximumLoan - player->loanBalance;
   
    lockEligibleCollateral(game, player);
   
    player->loanBalance = player->loanBalance + additionalAmount;
   
    player->cash = player->cash + additionalAmount;
    
    printf("%s increased the loan by LKR %lld.\n", player->name, additionalAmount);
    printf("New Loan Balance: LKR %lld\n", player->loanBalance);
    printf("Current Cash    : LKR %lld\n", player->cash);

    return 1;
}

void extendLoanPeriod(Player *player)
{
    if (player->hasActiveLoan == 0)
    {
        return;
    }

    player->loanRoundsRemaining = player->loanRoundsRemaining + LOAN_EXTENSION_ROUNDS;

    printf("%s extended the loan period.\n", player->name);
    printf("Rounds Remaining: %d\n", player->loanRoundsRemaining);
    
}

void refinanceLoan(Game *game, Player *player)
{
    if (player->hasActiveLoan == 0)
    {
        return;
    }

    Money refinancingFee = player->loanBalance * REFINANCE_FEE_PERCENT / 100;

    if (player->cash < refinancingFee)
    {
        printf("%s cannot afford the refinancing fee.\n", player->name);
        printf("Required Fee: LKR %lld\n", refinancingFee);

        return;
    }

    player->cash -= refinancingFee;

    player->loanInterestRate = getEffectiveBankLoanInterestRate(game);
    player->loanRoundsRemaining = LOAN_DURATION;

    printf("%s refinanced the loan.\n", player->name);
    printf("Refinancing Fee : LKR %lld\n", refinancingFee);
    printf("New Interest Rate: %d%%\n", player->loanInterestRate);
    printf("New Duration     : %d rounds\n", player->loanRoundsRemaining);
    printf("Remaining Cash   : LKR %lld\n", player->cash);

}

Money calculateProjectedRentalIncome(Game *game, Player *player)
{
    Money projectedIncome = 0;

    int railwayCount = countOwnedRailways(game, player->id);
    int utilityCount = countOwnedUtilities(game, player->id);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner != player->id || square->mortgaged == 1)
        {
            continue;
        }

        if (square->type == SQUARE_PROPERTY && square->damaged == 0)
        {
            Money rent = calculatePropertyRent(square);

            if (square->hasHotel == 1)
            {
                rent = rent * game->eventHotelRentMultiplier /100;
            }

            projectedIncome += rent;
        }
        else if (square->type == SQUARE_RAILWAY)
        {
            Money rent = calculateRailwayRent(railwayCount);
            rent = rent * game->eventRailwayRentMultiplier /100;
            rent = rent * game->governmentRailwayRentMultiplier /100;
            projectedIncome += rent;
        }
        else if (square->type == SQUARE_UTILITY)
        {
            Money rent = calculateUtilityRent(utilityCount, 7);
            rent = rent * game->governmentUtilityRentMultiplier /100;
            projectedIncome += rent;
        }
    }

    return projectedIncome;
}

void handleBankLanding(Game *game, Player *player)
{
    printf("\n==================================\n");
    printf("Bank of Ceylon\n");
    printf("==================================\n");

    Money maximumLoan = calculateMaximumLoan(game, player);

    printf("Current Cash      : LKR %lld\n", player->cash);
    printf("Maximum Loan      : LKR %lld\n", maximumLoan);
    printf("Current Bank Rate : %d%%\n", getEffectiveBankLoanInterestRate(game));

    switch (player->strategy)
    {
        case STRATEGY_AGGRESSIVE:
            
                if (player->hasActiveLoan == 0)
                {
                    obtainLoan(game, player, maximumLoan);
                    return;
                }

                if (player->cash > 2 * player->loanBalance)
                {
                    repayLoan(game, player, player->loanBalance);
                    return;
                }
                
                if (increaseLoanToMaximum(game, player) == 1)
                {
                    return;
                }

                printf("Aggressive Investor performs no Bank transaction.\n");
                return;
            
        
        case STRATEGY_CONSERVATIVE:
            
                if (player->hasActiveLoan == 0)
                {
                    return;
                }

                if (player->cash >= player->loanBalance)
                {
                    repayLoan(game, player, player->loanBalance);
                    return;
                }

                printf("Conservative Banker performs no Bank transaction.\n");
                return;
                

        case STRATEGY_RISK_TAKER:
            
                if (player->hasActiveLoan == 0)
                {
                    obtainLoan(game, player, maximumLoan);
                    return;
                }

                if (increaseLoanToMaximum(game, player) == 1)
                {
                    return;
                }

                if (player->loanRoundsRemaining <= 5)
                {
                    refinanceLoan(game, player);
                    return;
                }

                printf("Risk Taker performs no Bank transaction.\n");
                return;


        case STRATEGY_OPPORTUNISTIC:
                
            if (player->hasActiveLoan == 0)
            {
                Money projectedIncome = calculateProjectedRentalIncome(game, player);
                Money borrowingCost = maximumLoan * getEffectiveBankLoanInterestRate(game) /100;

                printf("Projected Rental Income: LKR %lld\n", projectedIncome);
                printf("Projected Borrowing Cost: LKR %lld\n", borrowingCost);

                if (maximumLoan > 0 && projectedIncome > borrowingCost)
                {
                    obtainLoan(game, player, maximumLoan);
                }
                else
                {
                    printf("Projected return does not justify a loan.\n");
                }

                return;
            }

            if (getEffectiveBankLoanInterestRate(game) < player->loanInterestRate)
            {
                refinanceLoan(game, player);
                return;
            }

            printf("Opportunistic Trader performs no Bank transaction.\n");
            return;

        default:
                return;
    }
}

int playerHasRemainingAssets(Game *game, Player *player)
{
    if (player->cash > 0)
    {
        return 1;
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && 
            (square->type == SQUARE_PROPERTY ||
            square->type == SQUARE_RAILWAY ||
            square->type == SQUARE_UTILITY))
        {
            return 1;
        }
    }

    return 0;
}

void forecloseLoan(Game *game, Player *player)
{
    printf("\n==================================\n");
    printf("LOAN DEFAULT\n");
    printf("==================================\n");

    printf("%s has defaulted on the loan.\n", player->name);

    int auctionSquares[BOARD_SIZE];
    int auctionCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && square->loanLocked == 1)
        {
            printf("%s has been foreclosed.\n", square->name);

            resetAssetForBank(square);

            auctionSquares[auctionCount] = i;
            auctionCount++;
        }
    }

    player->hasActiveLoan = 0;
    player->loanBalance = 0;
    player->loanInterestRate = 0;
    player->loanRoundsRemaining = 0;

    printf("Outstanding debt cleared.\n");

    if (playerHasRemainingAssets(game, player) == 0)
    {
        declareBankruptcy(game, player);
    }
    else
    {
        printf("%s continues the game with remaining assets.\n", player->name);
    }

    for (int i = 0; i < auctionCount; i++)
    {
        runAuction(game, &game->board[auctionSquares[i]]);
    }
}

void processEndOfRoundLoans(Game *game)
{
    int activeLoanFound = 0;

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        if (player->bankrupt == 1 || player->hasActiveLoan == 0)
        {
            continue;
        }

        if (activeLoanFound == 0)
        {
            printf("\n--- End of Round Loan Processing ---\n");
        }

        activeLoanFound = 1;

        int effectiveInterestRate = applyNationalLoanInterestModifiers(game, player, player->loanInterestRate);

        Money interest = player->loanBalance * effectiveInterestRate / 100;

        player->loanBalance += interest;

        player->loanRoundsRemaining--;

        printf("\n%s\n", player->name);
        printf("Effective Rate : %d%%\n", effectiveInterestRate);
        printf("Interest Added  : LKR %lld\n", interest);
        printf("Loan Balance    : LKR %lld\n", player->loanBalance);
        printf("Rounds Remaining: %d\n", player->loanRoundsRemaining);

        if (player->loanRoundsRemaining <= 0)
        {
            forecloseLoan(game, player);
        }
    }

    if (activeLoanFound == 0)
    {
        printf("No active loans to process.\n");
    }
}




InsuranceType chooseInsuranceType(Player *player, BoardSquare *square)
{
    int developed = square->houses > 0 || square->hasHotel == 1;

    if (developed == 0)
    {
        return INSURANCE_NONE;
    }

    switch (player->strategy)
    {

        case STRATEGY_AGGRESSIVE:
        {
            if (square->hasHotel == 1)
            {
                return INSURANCE_COMPREHENSIVE;
            }

            return INSURANCE_BASIC;
        }


        case STRATEGY_CONSERVATIVE:
            
            return INSURANCE_COMPREHENSIVE;


        case STRATEGY_RISK_TAKER:
        {
            if (player->experiencedFinancialLoss == 0)
            {
                return INSURANCE_NONE;
            }

            if (square->hasHotel == 1)
            {
                return INSURANCE_BUSINESS_INTERRUPTION;
            }

            return INSURANCE_COMPREHENSIVE;
        }


        case STRATEGY_OPPORTUNISTIC:
        {
            if (square->currentMarketValue >= HIGH_VALUE_PROPERTY_THRESHOLD)
            {
                return INSURANCE_COMPREHENSIVE;
            }

            return INSURANCE_NONE;
        }

        default:
            return INSURANCE_NONE;
    }
}

const char *insuranceTypeName(InsuranceType type)
{
    switch (type)
    {
        case INSURANCE_BASIC:
            return "Basic Property Insurance";

        case INSURANCE_COMPREHENSIVE:
            return "Comprehensive Insurance";

        case INSURANCE_BUSINESS_INTERRUPTION:
            return "Business Interruption Insurance";

        default:
            return "No Insurance";
    }
}

Money calculateInsurancePremium(Game *game, BoardSquare *square, InsuranceType type)
{
    int premiumPercent = 0;

    switch (type)
    {
        case INSURANCE_BASIC:

            premiumPercent = BASIC_PREMIUM_PERCENT;
            break;

        case INSURANCE_COMPREHENSIVE:

            premiumPercent = COMPREHENSIVE_PREMIUM_PERCENT;
            break;

        case INSURANCE_BUSINESS_INTERRUPTION:

            premiumPercent = BUSINESS_PREMIUM_PERCENT;
            break;

        default:
            return 0;
    }

    Money insuredValue = square->currentMarketValue;

    if (square->owner != NO_OWNER)
    {
        Player *owner = &game->players[square->owner];

        insuredValue = calculateNationalAdjustedAssetValue(game, owner, square);
    }

    Money premium = insuredValue * premiumPercent / 100;

    premium = premium * game->eventInsurancePremiumMultiplier/ 100;

    premium = premium * game->governmentInsuranceMultiplier/ 100;

    if (square->owner != NO_OWNER)
    {
        Player *owner = &game->players[square->owner];

    if (isNationalEventCardActive(game, owner, CARD_INSURANCE_DISCOUNT))
    {
        premium = premium * INSURANCE_DISCOUNT_MULTIPLIER/ 100;
    }
}

    return premium;
}

BoardSquare *findInsuranceCandidate(Game *game, Player *player)
{
    BoardSquare *selected = NULL;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY)
        {
            continue;
        }

        if (square->owner != player->id)
        {
            continue;
        }

        InsuranceType desiredType = chooseInsuranceType(player, square);

        if (desiredType == INSURANCE_NONE)
        {
            continue;
        }

        if (square->insuranceType !=
            INSURANCE_NONE)
        {
            int roundsRemaining = square->insuranceExpiryRound - game->currentRound;

            if (roundsRemaining > INSURANCE_REMINDER_ROUNDS)
            {
                continue;
            }
        }

        if (selected == NULL || square->currentMarketValue > selected->currentMarketValue)
        {
            selected = square;
        }
    }

    return selected;
}

int purchaseOrRenewInsurance(Game *game, Player *player, BoardSquare *square, InsuranceType type)
{
    if (type == INSURANCE_NONE)
    {
        return 0;
    }

    if (type == INSURANCE_BUSINESS_INTERRUPTION && square->hasHotel == 0)
    {
        printf("Business Interruption Insurance requires a hotel.\n");

        return 0;
    }

    Money premium = calculateInsurancePremium(game, square, type);

    if (player->cash < premium)
    {
        printf("%s cannot afford insurance for %s.\n", player->name, square->name);
        printf("Required Premium: LKR %lld\n", premium);

        return 0;
    }

    int renewing = square->insuranceType != INSURANCE_NONE;

    player->cash = player->cash - premium;

    square->insuranceType = type;

    square->insuranceExpiryRound = game->currentRound + INSURANCE_DURATION;

    square->insuranceReminderShown = 0;

    printf("\n%s %s.\n", insuranceTypeName(type), renewing ? "renewed" : "purchased");
    printf("Property : %s\n", square->name);
    printf("Premium  : LKR %lld\n", premium);
    printf("Expires after Round %d\n", square->insuranceExpiryRound);
    printf("Remaining Balance: LKR %lld\n", player->cash);

    return 1;
}

void handleInsuranceLanding(Game *game, Player *player)
{
    printf("\n==================================\n");
    printf("Insurance Company\n");
    printf("==================================\n");

    BoardSquare *property = findInsuranceCandidate(game, player);

    if (property == NULL)
    {
        printf("%s has no property requiring insurance at this time.\n", player->name);

        return;
    }

    InsuranceType type = chooseInsuranceType(player, property);

    printf("Selected Property : %s\n", property->name);
    printf("Selected Policy   : %s\n", insuranceTypeName(type));

    purchaseOrRenewInsurance(game, player, property, type);
}

void processInsuranceEndOfRound(Game *game)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->insuranceType == INSURANCE_NONE)
        {
            continue;
        }

        int roundsRemaining = square->insuranceExpiryRound - game->currentRound;

        if (roundsRemaining == INSURANCE_REMINDER_ROUNDS && square->insuranceReminderShown == 0)
        {
            printf("\nInsurance policy on %s expires in %d rounds.\n", square->name, INSURANCE_REMINDER_ROUNDS);

            square->insuranceReminderShown = 1;
        }

        if (roundsRemaining <= 0)
        {
            printf("\nInsurance policy on %s has expired.\n", square->name);

            square->insuranceType = INSURANCE_NONE;

            square->insuranceExpiryRound = 0;
            square->insuranceReminderShown = 0;
        }
    }
}



Money calculateRepairCost(BoardSquare *square)
{
    Money repairCost = square->currentMarketValue * DISASTER_REPAIR_PERCENT / 100;

    return repairCost;
}

int insuranceCoversDisaster(BoardSquare *square, DisasterType disaster)
{
    switch (square->insuranceType)
    {
        case INSURANCE_BASIC:

            return disaster == DISASTER_FIRE ||
                   disaster == DISASTER_FLOOD;

        case INSURANCE_COMPREHENSIVE:

            return disaster == DISASTER_FIRE ||
                   disaster == DISASTER_FLOOD ||
                   disaster == DISASTER_RIOT ||
                   disaster == DISASTER_VANDALISM ||
                   disaster == DISASTER_EARTHQUAKE;

        case INSURANCE_BUSINESS_INTERRUPTION:

            return square->hasHotel == 1;

        default:
            return 0;
    }
}

Money calculateInsuranceCompensation(Game *game, BoardSquare *square, DisasterType disaster, Money repairCost)
{
    if (insuranceCoversDisaster(square, disaster) == 0)
    {
        return 0;
    }

    switch (square->insuranceType)
    {
        case INSURANCE_BASIC:

            return repairCost *BASIC_COMPENSATION_PERCENT /100;

        case INSURANCE_COMPREHENSIVE:
            
            return repairCost;

        case INSURANCE_BUSINESS_INTERRUPTION:
        {
            Money hotelRent = calculatePropertyRent(square);
            Money lostRentalIncome = hotelRent * 5;
            Money compensation = repairCost + lostRentalIncome;
            compensation = compensation * game->businessClaimMultiplier /100;
            
            return compensation;
        }

        default:
            return 0;
    }
}

int attemptRepairProperty(Player *player, BoardSquare *square)
{
    if (square->damaged == 0)
    {
        return 1;
    }

    if (player->cash < square->repairCost)
    {
        printf("%s cannot currently afford to repair %s.\n", player->name, square->name);
        printf("Repair Cost: LKR %lld\n",square->repairCost);
        printf("The property remains closed.\n");

        return 0;
    }

    Money repairCost =square->repairCost;

    player->cash =player->cash - repairCost;

    square->damaged = 0;
    square->repairCost = 0;

    printf("%s repaired %s for LKR %lld.\n", player->name, square->name, repairCost);
    printf("%s can now collect rent again.\n", square->name);
    printf("Remaining Balance: LKR %lld\n", player->cash);

    return 1;
}

void repairOwnedDamagedProperties(Game *game, Player *player)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->owner == player->id && square->damaged == 1)
        {
            attemptRepairProperty(player, square);
        }
    }
}


int renovateProperty(Player *player, BoardSquare *square)
{
    if (square->damaged == 1)
    {
        printf("%s is damaged and must first "
               "be repaired.\n",
               square->name);

        return 0;
    }

    if (square->depreciationPercent == 0)
    {
        return 0;
    }

    Money renovationCost = square->currentMarketValue * RENOVATION_COST_PERCENT / 100;

    if (shouldRenovateProperty(player, square, renovationCost) == 0)
    {
        return 0;
    }

    player->cash = player->cash - renovationCost;
    square->propertyAge = 0;
    square->depreciationPercent = 0;

    recalculatePropertyAfterDepreciation(square);

    printf("\n%s renovated %s.\n", player->name, square->name);
    printf("Renovation Cost : LKR %lld\n",renovationCost);
    printf("Property Age    : %d\n", square->propertyAge);
    printf("Depreciation    : %d%%\n", square->depreciationPercent);
    printf("Restored Value  : LKR %lld\n", square->currentMarketValue);
    printf("Restored Rent   : LKR %lld\n", square->baseRent);
    printf("Remaining Cash  : LKR %lld\n", player->cash);

    return 1;
}

int getEffectiveBankLoanInterestRate(Game *game)
{
    int rate = (game->currentLoanInterestRate * game->eventLoanInterestMultiplier + 50) / 100;

    rate = rate + game->governmentLoanInterestAdjustment;

    if (rate < 0)
    {
        rate = 0;
    }

    return rate;
}

