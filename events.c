#include <stdio.h>
#include <stdlib.h>
#include "types.h"

const char *disasterTypeName(DisasterType disaster)
{
    switch (disaster)
    {
        case DISASTER_FIRE:
            return "Fire";

        case DISASTER_FLOOD:
            return "Flood"; 

        case DISASTER_RIOT:
            return "Riot";

        case DISASTER_BUILDING_COLLAPSE:
            return "Building Collapse";

        case DISASTER_ELECTRICAL_FAILURE:
            return "Electrical Failure";

        case DISASTER_VANDALISM:
            return "Vandalism";

        case DISASTER_EARTHQUAKE:
            return "Earthquake";

        default:
            return "Unknown Disaster";
    }
}

void processDisasterIfNeeded(Game *game)
{
    if (game->currentRound % 10 != 0)
    {
        return;
    }

    int candidates[BOARD_SIZE];
    int candidateCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY)
        {
            continue;
        }

        if (square->owner == NO_OWNER)
        {
            continue;
        }

        Player *owner = &game->players[square->owner];

        if (owner->bankrupt == 1)
        {
            continue;
        }

        int developed = square->houses > 0 || square->hasHotel == 1;

        if (developed == 0)
        {
            continue;
        }

        if (square->damaged == 1)
        {
            continue;
        }

        candidates[candidateCount] = i;
        candidateCount++;
    }

    if (candidateCount == 0)
    {
        printf("\nNo eligible developed property exists for a disaster.\n");

        return;
    }
 
    int fireWeight = 1;

    int floodWeight = game->floodRiskWeight;

    int riotWeight = game->riotRiskWeight;

    int collapseWeight = 1;
    int electricalWeight = 1;

    int totalWeight = fireWeight + floodWeight + riotWeight + collapseWeight + electricalWeight;

    int roll = rand() % totalWeight;
    
    DisasterType disaster;

    if (roll < fireWeight)
    {
        disaster = DISASTER_FIRE;
    }
    else if (roll < fireWeight + floodWeight)
    {
        disaster = DISASTER_FLOOD;
    }
    else if (roll < fireWeight + floodWeight + riotWeight)
    {
        disaster = DISASTER_RIOT;
    }
    else if (roll < fireWeight + floodWeight + riotWeight + collapseWeight)
    {
        disaster = DISASTER_BUILDING_COLLAPSE;
    }
    else
    {
        disaster = DISASTER_ELECTRICAL_FAILURE;
    }

int selected = rand() % candidateCount;

int squareIndex = candidates[selected];

    applyDisasterToProperty(game, &game->board[squareIndex], disaster);
}

void applyDisasterToProperty(Game *game, BoardSquare *square, DisasterType disaster)
{
    if (square->owner == NO_OWNER)
    {
        return;
    }

    Player *owner = &game->players[square->owner];

    Money repairCost = calculateRepairCost(square);

    Money compensation = calculateInsuranceCompensation(game, square, disaster, repairCost);

    printf("\n==================================\n");
    printf("DISASTER\n");
    printf("==================================\n");

    printf("%s occurred.\n", disasterTypeName(disaster));
    printf("Affected Property : %s\n", square->name);
    printf("Owner             : %s\n", owner->name);
    printf("Repair Cost       : LKR %lld\n", repairCost);

    if (compensation > 0)
    {
        owner->cash = owner->cash + compensation;

        printf("Insurance Claim Approved.\n");
        printf("Policy            : %s\n", insuranceTypeName(square->insuranceType));
        printf("Compensation Paid : LKR %lld\n", compensation);
    }
    else
    {
        printf("No insurance compensation is available.\n");
    }

    if (compensation < repairCost)
    {
        owner->experiencedFinancialLoss = 1;
    }

    square->damaged = 1;
    square->repairCost = repairCost;

    attemptRepairProperty(owner, square);
}



void recalculatePropertyAfterDepreciation(BoardSquare *square)
{
    int remainingPercent = 100 - square->depreciationPercent;

    square->currentMarketValue = square->normalMarketValue * remainingPercent / 100;
    square->baseRent = square->normalBaseRent * remainingPercent / 100;
}

Money applyPercentageChange(Money value, int percentageChange)
{
    Money newValue = value * (100 + percentageChange) /100;

    return newValue;
}

int generateInflationRate(void)
{
    int rates[6] = {-3, 0, 2, 5, 8, 12};

    int index = rand() % 6;

    return rates[index];
}

void updateBankInterestForInflation(Game *game, int inflationRate)
{
    int scaledRate = (int)game->currentLoanInterestRate * (100 + inflationRate);

    game->currentLoanInterestRate = (int)((scaledRate + 50) / 100);
}

void applyInflation(Game *game, int inflationRate)
{
    printf("\n==================================\n");
    printf("INFLATION UPDATE\n");
    printf("==================================\n");

    printf("New Inflation Rate: %+d%%\n", inflationRate);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY)
        {
            continue;
        }

        square->purchasePrice = applyPercentageChange(square->purchasePrice, inflationRate);
        square->normalMarketValue = applyPercentageChange(square->normalMarketValue,inflationRate);
        square->normalBaseRent = applyPercentageChange(square->normalBaseRent, inflationRate);

        square->houseCost =applyPercentageChange(square->houseCost, inflationRate);
        square->hotelCost = applyPercentageChange(square->hotelCost, inflationRate);

        recalculatePropertyAfterDepreciation(square);
    }

    updateBankInterestForInflation(game, inflationRate);

    game->currentInflationRate = inflationRate;

    printf("Current Bank Loan Rate: %d%%\n", game->currentLoanInterestRate);
}

void processInflationIfNeeded(Game *game)
{
    if (game->currentRound % 10 != 0)
    {
        return;
    }

    int inflationRate = generateInflationRate();

    applyInflation(game, inflationRate);
}

void processPropertyDepreciation(Game *game)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY)
        {
            continue;
        }

        square->propertyAge++;

        if (square->propertyAge <= DEPRECIATION_START_AGE)
        {
            continue;
        }

        if (square->propertyAge % DEPRECIATION_INTERVAL != 0)
        {
            continue;
        }

        if (square->depreciationPercent >= MAX_PROPERTY_DEPRECIATION)
        {
            continue;
        }

        square->depreciationPercent += DEPRECIATION_STEP_PERCENT;

        if (square->depreciationPercent > MAX_PROPERTY_DEPRECIATION)
        {
            square->depreciationPercent = MAX_PROPERTY_DEPRECIATION;
        }

        recalculatePropertyAfterDepreciation(square);

        printf("\nProperty %s has depreciated.\n", square->name);
        printf("Property Age       : %d rounds\n", square->propertyAge);
        printf("Total Depreciation : %d%%\n", square->depreciationPercent);
        printf("Current Value      : LKR %lld\n", square->currentMarketValue);
        printf("Current Base Rent  : LKR %lld\n", square->baseRent);
    }
}





void initializeEvents(Game *game)
{
    game->currentEconomicEvent = ECONOMIC_EVENT_NONE;
    game->currentRegulation = REGULATION_NONE;
    game->eventHotelRentMultiplier = NORMAL_MULTIPLIER;
    game->eventRailwayRentMultiplier = NORMAL_MULTIPLIER;
    game->eventHouseCostMultiplier = NORMAL_MULTIPLIER;
    game->eventHotelCostMultiplier = NORMAL_MULTIPLIER;
    game->eventInsurancePremiumMultiplier = NORMAL_MULTIPLIER;
    game->eventLoanInterestMultiplier = NORMAL_MULTIPLIER;
   
    game->floodRiskWeight = 1;
    game->riotRiskWeight = 1;
   
    game->businessClaimMultiplier = NORMAL_MULTIPLIER;
    game->governmentIncomeTaxMultiplier = NORMAL_MULTIPLIER;
    game->governmentLoanInterestAdjustment = 0;
    game->governmentHouseCostMultiplier = NORMAL_MULTIPLIER;
    game->governmentRailwayRentMultiplier = NORMAL_MULTIPLIER;
    game->governmentUtilityRentMultiplier = NORMAL_MULTIPLIER;
    game->governmentInsuranceMultiplier = NORMAL_MULTIPLIER;
   
    game->antiSpeculationActive = 0;
    initializeNationalEventDeck(game);
}

void initializeNationalEventDeck(Game *game)
{
    for (int i = 0; i < NATIONAL_EVENT_CARD_COUNT; i++)
    {
        game->nationalEventDeck[i] = (NationalEventCardType)i;
    }

    for (int i = NATIONAL_EVENT_CARD_COUNT - 1; i > 0; i--)
    {
        int randomIndex = rand() % (i + 1);

        NationalEventCardType temporary = game->nationalEventDeck[i];
        game->nationalEventDeck[i] = game->nationalEventDeck[randomIndex];
        game->nationalEventDeck[randomIndex] = temporary;
    }

    game->nextNationalEventCard = 0;
}

const char *nationalEventCardName(NationalEventCardType card)
{
    const char *names[NATIONAL_EVENT_CARD_COUNT] =
    {
        "Tourism Hype",
        "Fuel Shortage",
        "Heavy Floods",
        "Political Rally",
        "Stock Market Rise",
        "Economic Downturn",
        "Housing Subsidy",
        "Interest Rate Cut",
        "Interest Rate Increase",
        "Tax Amnesty",
        "Power Failure",
        "Foreign Funding",
        "Port Expansion",
        "Festival Season",
        "Labour Strike",
        "Insurance Discount",
        "Property Revaluation",
        "Currency Depreciation",
        "Government Grant",
        "National Disaster"
    };

    if (card < 0 || card >= NATIONAL_EVENT_CARD_COUNT)
    {
        return "Unknown National Event Card";
    }

    return names[card];
}

void drawNationalEventCard(Game *game, Player *player)
{
    NationalEventCardType card = game->nationalEventDeck[game->nextNationalEventCard];
    game->nextNationalEventCard = (game->nextNationalEventCard + 1) % NATIONAL_EVENT_CARD_COUNT;

    printf("\n==================================\n");
    printf("NATIONAL EVENT CARD\n");
    printf("==================================\n");

    printf("Player : %s\n", player->name);
    printf("Card   : %s\n", nationalEventCardName(card));

    applyNationalEventCard(game, player, card);

}



int isSouthernCoastalProperty(int index)
{
    return index == 26 ||
           index == 27 ||
           index == 29;
}

int isCoastalProperty(int index)
{
    return index == 6  ||
           index == 8  ||
           index == 9  ||
           index == 16 ||
           index == 26 ||
           index == 27 ||
           index == 29 ||
           index == 39;
}



const char *economicEventName(EconomicEventType event)
{
    switch (event)
    {
        case ECONOMIC_EVENT_TOURISM_BOOM:
            return "Tourism Boom";

        case ECONOMIC_EVENT_FUEL_CRISIS:
            return "Fuel Crisis";

        case ECONOMIC_EVENT_HEAVY_MONSOON:
            return "Heavy Monsoon";

        case ECONOMIC_EVENT_RECESSION:
            return "Economic Recession";

        case ECONOMIC_EVENT_STOCK_MARKET_BOOM:
            return "Stock Market Boom";

        case ECONOMIC_EVENT_HOUSING_PROGRAMME:
            return "Government Housing Programme";

        case ECONOMIC_EVENT_FOREIGN_INVESTMENT:
            return "Foreign Investment";

        case ECONOMIC_EVENT_POLITICAL_UNREST:
            return "Political Unrest";

        default:
            return "No Economic Event";
    }
}

void resetEconomicEventModifiers(Game *game)
{
    game->eventHotelRentMultiplier = NORMAL_MULTIPLIER;
    game->eventRailwayRentMultiplier = NORMAL_MULTIPLIER;
    game->eventHouseCostMultiplier = NORMAL_MULTIPLIER;
    game->eventHotelCostMultiplier = NORMAL_MULTIPLIER;
    game->eventInsurancePremiumMultiplier = NORMAL_MULTIPLIER;
    game->eventLoanInterestMultiplier = NORMAL_MULTIPLIER;

    game->floodRiskWeight = 1;
    game->riotRiskWeight = 1;

    game->businessClaimMultiplier = NORMAL_MULTIPLIER;
}

void changePropertyValue(BoardSquare *square, int percentage)
{
    square->purchasePrice = applyPercentageChange(square->purchasePrice, percentage);

    square->normalMarketValue = applyPercentageChange(square->normalMarketValue, percentage);
    
    recalculatePropertyAfterDepreciation(square);
}

void changePropertyRent(BoardSquare *square, int percentage)
{
    square->normalBaseRent = applyPercentageChange(square->normalBaseRent, percentage);

    recalculatePropertyAfterDepreciation(square);
}

void applyEconomicEvent(Game *game, EconomicEventType event)
{
    resetEconomicEventModifiers(game);

    game->currentEconomicEvent =
        event;

    printf("\n==================================\n");
    printf("ECONOMIC EVENT\n");
    printf("==================================\n");

    printf("%s\n",
           economicEventName(event));

    switch (event)
    {
        case ECONOMIC_EVENT_TOURISM_BOOM:
        {
            game->eventHotelRentMultiplier =
                TOURISM_HOTEL_RENT_MULTIPLIER;

            for (int i = 0;
                 i < BOARD_SIZE;
                 i++)
            {
                BoardSquare *square =
                    &game->board[i];

                if (square->type ==
                        SQUARE_PROPERTY &&
                    isSouthernCoastalProperty(i))
                {
                    changePropertyValue(
                        square,
                        15);
                }
            }

            printf("Hotels receive double rent.\n");
            printf("Southern coastal property "
                   "values increased by 15%%.\n");

            break;
        }

        case ECONOMIC_EVENT_FUEL_CRISIS:
        {
            game->eventRailwayRentMultiplier =
                FUEL_RAILWAY_RENT_MULTIPLIER;

            game->eventHouseCostMultiplier =
                FUEL_DEVELOPMENT_COST_MULTIPLIER;

            game->eventHotelCostMultiplier =
                FUEL_DEVELOPMENT_COST_MULTIPLIER;

            printf("Railway rent doubled.\n");
            printf("Development costs increased "
                   "by 20%%.\n");

            break;
        }

        case ECONOMIC_EVENT_HEAVY_MONSOON:
        {
            game->floodRiskWeight =
                MONSOON_FLOOD_RISK_WEIGHT;

            game->eventInsurancePremiumMultiplier =
                MONSOON_INSURANCE_MULTIPLIER;

            for (int i = 0;
                 i < BOARD_SIZE;
                 i++)
            {
                BoardSquare *square =
                    &game->board[i];

                if (square->type ==
                        SQUARE_PROPERTY &&
                    isCoastalProperty(i))
                {
                    changePropertyValue(
                        square,
                        -10);
                }
            }

            printf("Flood risk increased.\n");
            printf("Insurance premiums increased.\n");
            printf("Coastal property values "
                   "decreased by 10%%.\n");

            break;
        }

        case ECONOMIC_EVENT_RECESSION:
        {
            game->eventLoanInterestMultiplier =
                RECESSION_LOAN_INTEREST_MULTIPLIER;

            for (int i = 0;
                 i < BOARD_SIZE;
                 i++)
            {
                BoardSquare *square =
                    &game->board[i];

                if (square->type !=
                    SQUARE_PROPERTY)
                {
                    continue;
                }

                changePropertyValue(
                    square,
                    -15);

                changePropertyRent(
                    square,
                    -10);
            }

            printf("Property values decreased "
                   "by 15%%.\n");

            printf("Rent decreased by 10%%.\n");

            printf("New loan interest increased "
                   "by 15%%.\n");

            break;
        }

        case ECONOMIC_EVENT_STOCK_MARKET_BOOM:
        {
            game->eventLoanInterestMultiplier =
                STOCK_BOOM_LOAN_INTEREST_MULTIPLIER;

            for (int i = 0;
                 i < BOARD_SIZE;
                 i++)
            {
                BoardSquare *square =
                    &game->board[i];

                if (square->type ==
                    SQUARE_PROPERTY)
                {
                    changePropertyValue(
                        square,
                        10);
                }
            }

            printf("Property values increased "
                   "by 10%%.\n");

            printf("New loan interest decreased "
                   "by 10%%.\n");

            break;
        }

        case ECONOMIC_EVENT_HOUSING_PROGRAMME:
        {
            game->eventHouseCostMultiplier =
                HOUSING_PROGRAMME_COST_MULTIPLIER;

            printf("House construction costs "
                   "reduced by 25%%.\n");

            break;
        }

        case ECONOMIC_EVENT_FOREIGN_INVESTMENT:
        {
            for (int i = 0;
                 i < BOARD_SIZE;
                 i++)
            {
                BoardSquare *square =
                    &game->board[i];

                if (square->type ==
                        SQUARE_RAILWAY ||
                    square->type ==
                        SQUARE_UTILITY)
                {
                    square->purchasePrice =
                        applyPercentageChange(
                            square->purchasePrice,
                            20);

                    square->currentMarketValue =
                        applyPercentageChange(
                            square->currentMarketValue,
                            20);
                }
            }

            printf("Commercial asset values "
                   "increased by 20%%.\n");

            break;
        }

        case ECONOMIC_EVENT_POLITICAL_UNREST:
        {
            game->riotRiskWeight =
                POLITICAL_RIOT_RISK_WEIGHT;

            game->eventHotelRentMultiplier =
                POLITICAL_HOTEL_RENT_MULTIPLIER;

            game->businessClaimMultiplier =
                POLITICAL_BUSINESS_CLAIM_MULTIPLIER;

            printf("Riot probability doubled.\n");
            printf("Hotel rent reduced by 50%%.\n");
            printf("Business Interruption claims "
                   "increased.\n");

            break;
        }

        default:
            break;
    }
}

void processEconomicEventIfNeeded(Game *game)
{
    if (game->currentRound % ECONOMIC_EVENT_INTERVAL != 0)
    {
        return;
    }

    EconomicEventType event = (EconomicEventType)((rand() % 8) + 1);

    applyEconomicEvent(game, event);
}



const char *governmentRegulationName(GovernmentRegulationType regulation)
{
    switch (regulation)
    {
        case REGULATION_INCREASE_PROPERTY_TAX
        :
            return "Increase Property Tax";

        case REGULATION_REDUCE_LOAN_INTEREST:

            return "Reduce Loan Interest";

        case REGULATION_HOUSING_SUBSIDY:

            return "Housing Subsidy";

        case REGULATION_LUXURY_PROPERTY_TAX:

            return "Luxury Property Tax";

        case REGULATION_RAILWAY_MODERNIZATION:

    
            return "Railway Modernization";

        case REGULATION_ELECTRICITY_TARIFF:

            return "Electricity Tariff Revision";

        case REGULATION_INSURANCE:

            return "Insurance Regulation";

        case REGULATION_ANTI_SPECULATION:

            return "Anti-Speculation Act";

        default:
            return "No Government Regulation";
    }
}

void resetGovernmentRegulationModifiers(Game *game)
{
    game->governmentIncomeTaxMultiplier = NORMAL_MULTIPLIER;
    game->governmentLoanInterestAdjustment = 0;
    game->governmentHouseCostMultiplier = NORMAL_MULTIPLIER;
    game->governmentRailwayRentMultiplier = NORMAL_MULTIPLIER;
    game->governmentUtilityRentMultiplier = NORMAL_MULTIPLIER;
    game->governmentInsuranceMultiplier = NORMAL_MULTIPLIER;
    game->antiSpeculationActive = 0;
}

void applyAntiSpeculationToExistingProperties(Game *game)
{
    for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++)
    {
        Player *player = &game->players[playerIndex];

        int undevelopedCount = 0;

        if (player->bankrupt == 1)
        {
            continue;
        }

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            BoardSquare *square = &game->board[i];

            if (square->type != SQUARE_PROPERTY ||
                square->owner != player->id ||
                square->houses > 0 ||
                square->hasHotel == 1)
            {
                continue;
            }

            undevelopedCount++;

            if (undevelopedCount > ANTI_SPECULATION_MAX_UNDEVELOPED && square->developmentDeadlineRound == 0)
            {
                square->developmentDeadlineRound = game->currentRound + ANTI_SPECULATION_DEVELOPMENT_ROUNDS;

                printf("%s must develop %s before Round %d.\n", player->name, square->name, square->developmentDeadlineRound);
            }
        }
    }
}

void chargeLuxuryPropertyTax(Game *game)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type != SQUARE_PROPERTY)
        {
            continue;
        }

        if (square->hasHotel == 0)
        {
            continue;
        }

        if (square->owner == NO_OWNER)
        {
            continue;
        }

        Player *owner = &game->players[square->owner];

        Money tax = square->currentMarketValue * 25 / 100;

        printf("\nLuxury Property: %s\n", square->name);

        payTax(game, owner, tax, "Luxury Property Tax");
    }
}

void applyGovernmentRegulation(Game *game, GovernmentRegulationType regulation)
{
    resetGovernmentRegulationModifiers(game);

    game->currentRegulation = regulation;

    printf("\n==================================\n");
    printf("GOVERNMENT REGULATION\n");
    printf("==================================\n");

    printf("%s\n", governmentRegulationName(regulation));

    switch (regulation)
    {
        case REGULATION_INCREASE_PROPERTY_TAX:

            game->governmentIncomeTaxMultiplier = GOVERNMENT_PROPERTY_TAX_MULTIPLIER;

            printf("Income Tax increased by 50%%.\n");
            break;


        case REGULATION_REDUCE_LOAN_INTEREST:
            game->governmentLoanInterestAdjustment = -GOVERNMENT_LOAN_INTEREST_REDUCTION;

            printf("New loan interest reduced by 2 percentage points.\n");
            break;


        case REGULATION_HOUSING_SUBSIDY:
            game->governmentHouseCostMultiplier =
                GOVERNMENT_HOUSING_COST_MULTIPLIER;

            printf("House construction costs "
                   "reduced by 30%%.\n");
            break;


        case REGULATION_LUXURY_PROPERTY_TAX:

            printf("Hotel properties are charged 25%% Luxury Property Tax.\n");
            chargeLuxuryPropertyTax(game);

            break;


        case REGULATION_RAILWAY_MODERNIZATION:

            game->governmentRailwayRentMultiplier = GOVERNMENT_RAILWAY_RENT_MULTIPLIER;

            printf("Railway rents increased by 25%%.\n");
            break;


        case REGULATION_ELECTRICITY_TARIFF: 
             
            game->governmentUtilityRentMultiplier = GOVERNMENT_UTILITY_RENT_MULTIPLIER;

            printf("Utility rents increased by 20%%.\n");
            break;


        case REGULATION_INSURANCE:
            game->governmentInsuranceMultiplier = GOVERNMENT_INSURANCE_MULTIPLIER;

            printf("Insurance premiums reduced by 15%%.\n");
            break;


        case REGULATION_ANTI_SPECULATION:
            
            game->antiSpeculationActive = 1;

            printf("Players may own at most three undeveloped properties.\n");
            printf("Additional undeveloped properties must be developed within five rounds.\n");
            
            applyAntiSpeculationToExistingProperties(game);
            
            break;


        default:
            break;
    }
}

void processGovernmentRegulationIfNeeded(Game *game)
{
    if (game->currentRound % GOVERNMENT_REGULATION_INTERVAL != 0)
    {
        return;
    }

    GovernmentRegulationType regulation = (GovernmentRegulationType)((rand() % 8) + 1);

    applyGovernmentRegulation(game,regulation);
}

Money getCurrentHouseCost(Game *game, BoardSquare *square)
{
    Money cost = square->houseCost;

    cost = cost * game->eventHouseCostMultiplier /100;

    cost = cost * game->governmentHouseCostMultiplier /100;

    return cost;
}

Money getCurrentHotelCost(Game *game, BoardSquare *square)
{
    Money cost = square->hotelCost;

    cost =cost * game->eventHotelCostMultiplier /100;

    return cost;
}

int countUndevelopedProperties(Game *game, Player *player)
{
    int count = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY &&
            square->owner == player->id &&
            square->houses == 0 &&
            square->hasHotel == 0)
        {
            count++;
        }
    }

    return count;
}

void applyAntiSpeculationDeadline(Game *game, Player *player, BoardSquare *square)
{
    if (game->antiSpeculationActive == 0)
    {
        return;
    }

    if (square->type != SQUARE_PROPERTY)
    {
        return;
    }

    if (square->houses > 0 || square->hasHotel == 1)
    {
        return;
    }

    int undevelopedCount = countUndevelopedProperties(game, player);

    if (undevelopedCount <= ANTI_SPECULATION_MAX_UNDEVELOPED)
    {
        return;
    }

    square->developmentDeadlineRound = game->currentRound + ANTI_SPECULATION_DEVELOPMENT_ROUNDS;

    printf("\nAnti-Speculation Act applies to %s.\n", square->name);

    printf("Development Deadline: Round %d\n", square->developmentDeadlineRound);
}

void processAntiSpeculationDeadlines(Game *game)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->developmentDeadlineRound == 0)
        {
            continue;
        }

        if (square->houses > 0 || square->hasHotel == 1)
        {
            square->developmentDeadlineRound = 0;
            continue;
        }

        if (game->currentRound < square->developmentDeadlineRound)
        {
            continue;
        }

        if (square->owner == NO_OWNER)
        {
            square->developmentDeadlineRound = 0;
            continue;
        }

        printf("\n==================================\n");
        printf("ANTI-SPECULATION VIOLATION\n");
        printf("==================================\n");

        printf("\n%s was not developed before Round %d.\n", square->name, square->developmentDeadlineRound);

        Player *owner = &game->players[square->owner];

        if (square->loanLocked == 1)
        {
            printf("The property was pledged as loan collateral.\n");
            printf("The missed deadline causes loan foreclosure.\n");

            forecloseLoan(game, owner);

            continue;
        }

        printf("The property returns to the Bank.\n");

        resetAssetForBank(square);

        runAuction(game, square);
    }
}



void giveTaxAmnesty(Game *game)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        if (player->bankrupt == 1)
        {
            continue;
        }

        player->cash += 2000;
        printf("%s received LKR 2000. New cash: LKR %lld\n", player->name, player->cash);
    }
}

void giveGovernmentGrant(Game *game)
{
    int candidates[PLAYER_COUNT];
    int candidateCount = 0;

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        if (game->players[i].bankrupt == 0)
        {
            candidates[candidateCount] = i;
            candidateCount++;
        }
    }

    if (candidateCount == 0)
    {
        printf("No solvent player can receive the grant.\n");
        return;
    }

    int selectedPlayerId = candidates[rand() % candidateCount];

    Player *selectedPlayer = &game->players[selectedPlayerId];

    selectedPlayer->cash += 5000;

    printf("%s received the Government Grant of LKR 5000.\n", selectedPlayer->name);
    printf("New Cash : LKR %lld\n", selectedPlayer->cash);
}

void damageRandomCoastalProperty(Game *game)
{
    int candidates[BOARD_SIZE];
    int candidateCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY &&
            isCoastalProperty(i) == 1 &&
            square->owner != NO_OWNER &&
            square->damaged == 0 &&
            game->players[square->owner].bankrupt == 0)
        {
            candidates[candidateCount] = i;
            candidateCount++;
        }
    }

    if (candidateCount == 0)
    {
        printf("No eligible owned coastal property was found.\n");

        return;
    }

    int selectedIndex =candidates[rand() % candidateCount];

    applyDisasterToProperty(game, &game->board[selectedIndex], DISASTER_FLOOD);
}

void damageRandomDevelopedProperty(Game *game)
{
    int candidates[BOARD_SIZE];
    int candidateCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY &&
            square->owner != NO_OWNER &&
            square->damaged == 0 &&
            (square->houses > 0 ||
             square->hasHotel == 1) &&
            game->players[square->owner].bankrupt == 0)
        {
            candidates[candidateCount] = i;
            candidateCount++;
        }
    }

    if (candidateCount == 0)
    {
        printf("No eligible developed property was found.\n");

        return;
    }

    int selectedIndex = candidates[rand() % candidateCount];

    DisasterType disaster = (DisasterType)(rand() % 5);

    applyDisasterToProperty(game, &game->board[selectedIndex], disaster
    );
}



int getNationalEventCardDuration(NationalEventCardType card)
{
    switch (card)
    {
        case CARD_TOURISM_HYPE:

            return TOURISM_HYPE_DURATION;

        case CARD_FUEL_SHORTAGE:

            return FUEL_SHORTAGE_DURATION;

        case CARD_POLITICAL_RALLY:

            return POLITICAL_RALLY_DURATION;

        case CARD_POWER_FAILURE:

            return POWER_FAILURE_DURATION;

        case CARD_LABOUR_STRIKE:

            return LABOUR_STRIKE_DURATION;

        case CARD_HEAVY_FLOODS:
        case CARD_TAX_AMNESTY:
        case CARD_GOVERNMENT_GRANT:
        case CARD_NATIONAL_DISASTER:

            return 0;

        default:

            return NATIONAL_EVENT_DEFAULT_DURATION;
    }
}

int isNationalEventCardActive(Game *game, Player *player, NationalEventCardType card)
{
    return player->nationalCardExpiry[card] >= game->currentRound;
}

Money applyNationalConstructionCostModifiers(Game *game, Player *player, Money cost, int isHouse)
{
    if (isHouse == 1 &&
        isNationalEventCardActive(game, player, CARD_HOUSING_SUBSIDY))
    {
        cost = cost * HOUSING_SUBSIDY_COST_MULTIPLIER/ 100;
    }

    if (isNationalEventCardActive(game, player, CARD_CURRENCY_DEPRECIATION))
    {
        cost = cost * CURRENCY_DEPRECIATION_COST_MULTIPLIER/ 100;
    }

    return cost;
}

int applyNationalLoanInterestModifiers(Game *game, Player *player, int interestRate)
{
    if (isNationalEventCardActive(game, player, CARD_INTEREST_RATE_CUT))
    {
        interestRate -= NATIONAL_CARD_INTEREST_ADJUSTMENT;
    }

    if (isNationalEventCardActive(game, player, CARD_INTEREST_RATE_INCREASE))
    {
        interestRate += NATIONAL_CARD_INTEREST_ADJUSTMENT;
    }

    if (interestRate < 0)
    {
        interestRate = 0;
    }

    return interestRate;
}

Money calculateNationalAdjustedAssetValue(Game *game, Player *player, BoardSquare *square)
{
    Money value = square->currentMarketValue;

    if (square->type == SQUARE_PROPERTY)
    {
        if (isNationalEventCardActive(game, player, CARD_STOCK_MARKET_RISE))
        {
            value = value * STOCK_MARKET_RISE_VALUE_MULTIPLIER/ 100;
        }

        if (isNationalEventCardActive(game, player, CARD_ECONOMIC_DOWNTURN))
        {
            value = value * ECONOMIC_DOWNTURN_VALUE_MULTIPLIER/ 100;
        }

        if (isNationalEventCardActive(game, player, CARD_PROPERTY_REVALUATION) &&
            square->group == player->revaluationGroup)
        {
            value = value * PROPERTY_REVALUATION_VALUE_MULTIPLIER/ 100;
        }
    }

    if ((square->type == SQUARE_RAILWAY ||
         square->type == SQUARE_UTILITY) &&
         isNationalEventCardActive(game, player, CARD_FOREIGN_FUNDING))
    {
        value = value * FOREIGN_FUNDING_VALUE_MULTIPLIER/ 100;
    }

    if (square->type == SQUARE_RAILWAY && isNationalEventCardActive(game, player, CARD_PORT_EXPANSION))
    {
        value = value * PORT_EXPANSION_VALUE_MULTIPLIER/ 100;
    }

    return value;
}

void activateNationalEventCard(Game *game, Player *player, NationalEventCardType card)
{
    int duration = getNationalEventCardDuration(card);

    if (duration <= 0)
    {
        return;
    }

    player->nationalCardExpiry[card] = game->currentRound + duration - 1;

    printf("Effect Duration : %d rounds\n", duration);
    printf("Expires after Round %d\n", player->nationalCardExpiry[card]);
}

void closeRandomProperty(Game *game)
{
    int candidates[BOARD_SIZE];
    int candidateCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->type == SQUARE_PROPERTY &&
            square->owner != NO_OWNER &&
            square->damaged == 0 &&
            square->closedUntilRound <
            game->currentRound &&
            game->players[square->owner].bankrupt == 0)
        {
            candidates[candidateCount] = i;
            candidateCount++;
        }
    }

    if (candidateCount == 0)
    {
        printf("No eligible property can be closed.\n");
        return;
    }

    int selectedIndex = candidates[rand() % candidateCount];

    BoardSquare *selected = &game->board[selectedIndex];

    selected->closedUntilRound = game->currentRound + POLITICAL_RALLY_DURATION - 1;

    printf("Closed Property : %s\n", selected->name);
    printf("Closed through Round %d\n",selected->closedUntilRound);
}

const char *nationalPropertyGroupName(PropertyGroup group)
{
    const char *names[8] =
    {
        "Brown",
        "Light Blue",
        "Pink",
        "Orange",
        "Red",
        "Yellow",
        "Green",
        "Dark Blue"
    };

    if (group < GROUP_BROWN ||
        group > GROUP_DARK_BLUE)
    {
        return "Unknown Group";
    }

    return names[group];
}

void selectRevaluationGroup(Player *player)
{
    player->revaluationGroup = (PropertyGroup)(rand() % 8);

    printf("Revalued Group : %s\n", nationalPropertyGroupName(player->revaluationGroup));
}

void applyNationalEventCard(Game *game, Player *player, NationalEventCardType card)
{
    switch (card)
    {
        case CARD_HEAVY_FLOODS:

            damageRandomCoastalProperty(game);
            break;

        case CARD_TAX_AMNESTY:

            giveTaxAmnesty(game);
            break;

        case CARD_GOVERNMENT_GRANT:

            giveGovernmentGrant(game);
            break;

        case CARD_NATIONAL_DISASTER:

            damageRandomDevelopedProperty(game);
            break;

        case CARD_POLITICAL_RALLY:

            activateNationalEventCard(game, player, card);
            closeRandomProperty(game);
            break;

        case CARD_PROPERTY_REVALUATION:

            selectRevaluationGroup(player);
            activateNationalEventCard(game, player, card);
            break;

        default:

            activateNationalEventCard(game, player, card);
            break;
    }
}

void displayActiveNationalEventCards(Game *game)
{
    int found = 0;

    printf("\n--- Active National Event Cards ---\n");

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        if (player->bankrupt == 1)
        {
            continue;
        }

        for (int card = 0; card < NATIONAL_EVENT_CARD_COUNT; card++)
        {
            if (player->nationalCardExpiry[card] < game->currentRound)
            {
                continue;
            }

            int roundsRemaining = player->nationalCardExpiry[card] - game->currentRound + 1;

            printf("%s : %s | Rounds remaining: %d\n", player->name, nationalEventCardName((NationalEventCardType)card), roundsRemaining);

            found = 1;
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->closedUntilRound >= game->currentRound)
        {
            int roundsRemaining = square->closedUntilRound - game->currentRound + 1;

            printf("Closed Property : %s | Rounds remaining: %d\n", square->name, roundsRemaining);

            found = 1;
        }
    }

    if (found == 0)
    {
        printf("No National Event effects are active.\n");
    }
}

void processNationalEventCardsEndOfRound(Game *game)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Player *player = &game->players[i];

        for (int card = 0; card < NATIONAL_EVENT_CARD_COUNT; card++)
        {
            if (player->nationalCardExpiry[card] == game->currentRound)
            {
                printf("%s's %s effect has expired.\n", player->name, nationalEventCardName((NationalEventCardType)card));

                player->nationalCardExpiry[card] = 0;

                if (card == CARD_PROPERTY_REVALUATION)
                {
                    player->revaluationGroup = GROUP_NONE;
                }
            }
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        BoardSquare *square = &game->board[i];

        if (square->closedUntilRound == game->currentRound)
        {
            printf("%s has reopened.\n", square->name);

            square->closedUntilRound = 0;
        }
    }
}

