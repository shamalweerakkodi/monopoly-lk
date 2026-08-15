#ifndef TYPES_H                          //define types.h if not defined
#define TYPES_H

#define BOARD_SIZE 40
#define PLAYER_COUNT 4
#define MAX_ROUNDS 500
#define STARTING_CASH 30000
#define GO_REWARD 2000
#define COMMUNITY_FUND_POSITION 2
#define JAIL_POSITION 10
#define GO_TO_JAIL_POSITION 30
#define JAIL_BAIL 300

#define NO_OWNER -1

#define INCOME_TAX_BASE_RATE 15
#define COMMUNITY_FUND_BASE_RATE 10

#define MAX_LOAN_PERCENT 75
#define REFINANCE_FEE_PERCENT 2
#define LOAN_DURATION 20
#define LOAN_EXTENSION_ROUNDS 10
#define STABLE_LOAN_INTEREST_RATE 8
#define RAILWAY_MORTGAGE_VALUE 750
#define UTILITY_MORTGAGE_VALUE 750

#define AGGRESIVE_RENT_RESERVE 2000
#define RAILWAY_PURCHASE_PRICE 1500
#define UTILITY_PURCHASE_PRICE 1500

#define AUCTION_INCREMENT 250
#define AUCTION_START_PERCENT 50
#define AGGRESSIVE_AUCTION_LIMIT_PERCENT 120
#define OPPORTUNISTIC_AUCTION_LIMIT_PERCENT 90

#define INSURANCE_DURATION 20
#define INSURANCE_REMINDER_ROUNDS 3
#define BASIC_PREMIUM_PERCENT 5
#define COMPREHENSIVE_PREMIUM_PERCENT 10
#define BUSINESS_PREMIUM_PERCENT 15

#define BASIC_COMPENSATION_PERCENT 80
#define DISASTER_REPAIR_PERCENT 20

#define HIGH_VALUE_PROPERTY_THRESHOLD 8000

#define DEPRECIATION_START_AGE 50
#define DEPRECIATION_INTERVAL 5
#define DEPRECIATION_STEP_PERCENT 1
#define MAX_PROPERTY_DEPRECIATION 30 
#define RENOVATION_COST_PERCENT 10
#define AGGRESSIVE_RENOVATION_THRESHOLD 20
#define RISK_TAKER_RENOVATION_THRESHOLD 30



#define ECONOMIC_EVENT_INTERVAL 15
#define GOVERNMENT_REGULATION_INTERVAL 20
#define NATIONAL_EVENT_CARD_COUNT 20
#define TOURISM_HYPE_RENT_MULTIPLIER 200
#define FUEL_SHORTAGE_RENT_MULTIPLIER 200
#define POWER_FAILURE_RENT_MULTIPLIER 50
#define FESTIVAL_SEASON_RENT_MULTIPLIER 150

#define HOUSING_SUBSIDY_COST_MULTIPLIER 70
#define CURRENCY_DEPRECIATION_COST_MULTIPLIER 110
#define INSURANCE_DISCOUNT_MULTIPLIER 80

#define STOCK_MARKET_RISE_VALUE_MULTIPLIER 110
#define ECONOMIC_DOWNTURN_VALUE_MULTIPLIER 85
#define FOREIGN_FUNDING_VALUE_MULTIPLIER 115
#define PORT_EXPANSION_VALUE_MULTIPLIER 120
#define PROPERTY_REVALUATION_VALUE_MULTIPLIER 115

#define NATIONAL_CARD_INTEREST_ADJUSTMENT 2



#define NATIONAL_EVENT_DEFAULT_DURATION 15

#define TOURISM_HYPE_DURATION 5
#define FUEL_SHORTAGE_DURATION 5
#define POLITICAL_RALLY_DURATION 2
#define POWER_FAILURE_DURATION 3
#define LABOUR_STRIKE_DURATION 2

#define NORMAL_MULTIPLIER 100

#define TOURISM_HOTEL_RENT_MULTIPLIER 200

#define FUEL_RAILWAY_RENT_MULTIPLIER 200
#define FUEL_DEVELOPMENT_COST_MULTIPLIER 120

#define MONSOON_FLOOD_RISK_WEIGHT 2

#define MONSOON_INSURANCE_MULTIPLIER 125
#define RECESSION_LOAN_INTEREST_MULTIPLIER 115
#define STOCK_BOOM_LOAN_INTEREST_MULTIPLIER 90
#define HOUSING_PROGRAMME_COST_MULTIPLIER 75

#define POLITICAL_RIOT_RISK_WEIGHT 2
#define POLITICAL_HOTEL_RENT_MULTIPLIER 50

#define POLITICAL_BUSINESS_CLAIM_MULTIPLIER 150
#define GOVERNMENT_PROPERTY_TAX_MULTIPLIER 150

#define GOVERNMENT_LOAN_INTEREST_REDUCTION 2

#define GOVERNMENT_HOUSING_COST_MULTIPLIER 70
#define GOVERNMENT_RAILWAY_RENT_MULTIPLIER 125
#define GOVERNMENT_UTILITY_RENT_MULTIPLIER 120
#define GOVERNMENT_INSURANCE_MULTIPLIER 85

#define ANTI_SPECULATION_MAX_UNDEVELOPED 3
#define ANTI_SPECULATION_DEVELOPMENT_ROUNDS 5

#define BUILDING_SELL_PERCENT 50



typedef long long Money;

                                     //dice roll structure
typedef struct {
    
    int die1;
    int die2;
    int total;
    int isDouble;

}DiceRoll;

                                           //square types for the board squares
typedef enum {
    SQUARE_GO,
    SQUARE_PROPERTY,
    SQUARE_EVENT,
    SQUARE_TAX,
    SQUARE_RAILWAY,
    SQUARE_JAIL,
    SQUARE_UTILITY,
    SQUARE_INSURANCE,
    SQUARE_FREE_PARKING,
    SQUARE_GO_TO_JAIL,
    SQUARE_BANK
} SquareType;

                                        //property groups for the properties
typedef enum {
    GROUP_NONE = -1,
    GROUP_BROWN,
    GROUP_LIGHT_BLUE,
    GROUP_PINK,
    GROUP_ORANGE,
    GROUP_RED,
    GROUP_YELLOW,
    GROUP_GREEN,
    GROUP_DARK_BLUE
} PropertyGroup;

                                         //strategy types for the players
typedef enum {
    STRATEGY_AGGRESSIVE,
    STRATEGY_CONSERVATIVE,
    STRATEGY_RISK_TAKER,
    STRATEGY_OPPORTUNISTIC
} StrategyType;

typedef enum
{
    INSURANCE_NONE,
    INSURANCE_BASIC,
    INSURANCE_COMPREHENSIVE,
    INSURANCE_BUSINESS_INTERRUPTION

} InsuranceType;

typedef enum
{
    DISASTER_FIRE,
    DISASTER_FLOOD,
    DISASTER_RIOT,
    DISASTER_BUILDING_COLLAPSE,
    DISASTER_ELECTRICAL_FAILURE,
    DISASTER_VANDALISM,
    DISASTER_EARTHQUAKE

} DisasterType;

typedef enum
{
    ECONOMIC_EVENT_NONE,
    ECONOMIC_EVENT_TOURISM_BOOM,
    ECONOMIC_EVENT_FUEL_CRISIS,
    ECONOMIC_EVENT_HEAVY_MONSOON,
    ECONOMIC_EVENT_RECESSION,
    ECONOMIC_EVENT_STOCK_MARKET_BOOM,
    ECONOMIC_EVENT_HOUSING_PROGRAMME,
    ECONOMIC_EVENT_FOREIGN_INVESTMENT,
    ECONOMIC_EVENT_POLITICAL_UNREST

} EconomicEventType;

typedef enum
{
    REGULATION_NONE,
    REGULATION_INCREASE_PROPERTY_TAX,
    REGULATION_REDUCE_LOAN_INTEREST,
    REGULATION_HOUSING_SUBSIDY,
    REGULATION_LUXURY_PROPERTY_TAX,
    REGULATION_RAILWAY_MODERNIZATION,
    REGULATION_ELECTRICITY_TARIFF,
    REGULATION_INSURANCE,
    REGULATION_ANTI_SPECULATION

} GovernmentRegulationType;

typedef enum
{
    CARD_TOURISM_HYPE,
    CARD_FUEL_SHORTAGE,
    CARD_HEAVY_FLOODS,
    CARD_POLITICAL_RALLY,
    CARD_STOCK_MARKET_RISE,
    CARD_ECONOMIC_DOWNTURN,
    CARD_HOUSING_SUBSIDY,
    CARD_INTEREST_RATE_CUT,
    CARD_INTEREST_RATE_INCREASE,
    CARD_TAX_AMNESTY,
    CARD_POWER_FAILURE,
    CARD_FOREIGN_FUNDING,
    CARD_PORT_EXPANSION,
    CARD_FESTIVAL_SEASON,
    CARD_LABOUR_STRIKE,
    CARD_INSURANCE_DISCOUNT,
    CARD_PROPERTY_REVALUATION,
    CARD_CURRENCY_DEPRECIATION,
    CARD_GOVERNMENT_GRANT,
    CARD_NATIONAL_DISASTER
} NationalEventCardType;
                                         //board square structure  
typedef struct 
{
    int index;
    const char *name;

    SquareType type;
    PropertyGroup group;

    Money purchasePrice;
    Money baseRent;

    Money houseCost;
    Money hotelCost;
    Money mortgageValue;

    Money currentMarketValue;

    InsuranceType insuranceType;
    int insuranceExpiryRound;
    int insuranceReminderShown;
    int damaged;
    Money repairCost;

    int closedUntilRound;

    int houses;
    int hasHotel;
    int mortgaged;
    int loanLocked;

    Money normalMarketValue;
    Money normalBaseRent;

    int propertyAge;
    int depreciationPercent; 
    
    int developmentDeadlineRound;

    int owner;  
} BoardSquare;
                                     //player structure
typedef struct {
    int id;
    const char *name;
    StrategyType strategy;

    Money cash;
    Money taxDue;
    
    int position;
    int bankrupt;
    int inJail;
    int jailTurns;

    int hasActiveLoan;
    Money loanBalance;
    int loanInterestRate;
    int loanRoundsRemaining;
    int experiencedFinancialLoss;

    int nationalCardExpiry[NATIONAL_EVENT_CARD_COUNT];
    PropertyGroup revaluationGroup;

} Player;
                                                 //game structure
typedef struct {
    BoardSquare board[BOARD_SIZE];
    Player players[PLAYER_COUNT];

    int turnOrder[PLAYER_COUNT];
    int currentRound;

    int incomeTaxRate;
    int communityFundTaxRate;

    int currentLoanInterestRate;
    int currentInflationRate;

    EconomicEventType currentEconomicEvent;
    GovernmentRegulationType currentRegulation;

    /* Economic Event modifiers */
    int eventHotelRentMultiplier;
    int eventRailwayRentMultiplier;

    int eventHouseCostMultiplier;
    int eventHotelCostMultiplier;

    int eventInsurancePremiumMultiplier;
    int eventLoanInterestMultiplier;

    int floodRiskWeight;
    int riotRiskWeight;

    int businessClaimMultiplier;


    /* Government Regulation modifiers */
    int governmentIncomeTaxMultiplier;

    int governmentLoanInterestAdjustment;

    int governmentHouseCostMultiplier;
    int governmentRailwayRentMultiplier;
    int governmentUtilityRentMultiplier;
    int governmentInsuranceMultiplier;

    int antiSpeculationActive;

    NationalEventCardType nationalEventDeck[NATIONAL_EVENT_CARD_COUNT];
    int nextNationalEventCard;
} Game;

void initializePlayers(Game *game);                          //player initialization function
void initializeBoard(Game *game);                            //board initialization

DiceRoll rollDice(void);                                  //dice roll function

int movePlayer(Player *player, int diceTotal);          //player movement function

void displayLanding(Game *game, Player *player);        //lading square details

void determineTurnOrder(Game *game);                    //player turn order

void sendPlayerToJail(Player *player);                  //jail actions

void resolveLanding(Game *game, Player *player, int diceTotal);        //landing actions

void handlePropertyLanding(Game *game, Player *player, BoardSquare *square);      //purchase land

int findLastActivePlayer(Game *game);

int playBasicTurn(Game *game, Player *player);         //roll dice, move player, display landing



void payPropertyRent(Game *game, Player *player, BoardSquare *square);

void handleRailwayLanding(Game *game, Player *visitor, BoardSquare *square);

void handleUtilityLanding(Game *game, Player *visitor, BoardSquare *square, int diceTotal);

int shouldBuySquare(Player *player, BoardSquare *square);                   //yes or no

int purchaseSquare(Player *player, BoardSquare *square);                    //buy squares

int shouldBidInAuction(Player *player, Money nextBid, Money marketValue);

void runAuction(Game *game, BoardSquare *square);



void initializeFinance(Game *game);

void handleIncomeTax(Game *game, Player *player);

void handleCommunityDevelopmentFund(Game *game, Player *player);

int ownsMonopoly(Game *game, Player *player, PropertyGroup group);

void developProperties(Game *game, Player *player);

void extendLoanPeriod(Player *player);

void handleBankLanding(Game *game, Player *player);

void processEndOfRoundLoans(Game *game);

void forecloseLoan(Game *game, Player *player);



InsuranceType chooseInsuranceType(Player *player, BoardSquare *square);

Money calculateInsurancePremium(Game *game, BoardSquare *square, InsuranceType type);

Money calculateInsuranceCompensation(Game *game, BoardSquare *square, DisasterType disaster, Money repairCost);

void handleInsuranceLanding(Game *game,Player *player);

void processInsuranceEndOfRound(Game *game);

Money calculateRepairCost(BoardSquare *square);

int insuranceCoversDisaster(BoardSquare *square, DisasterType disaster);

void applyDisasterToProperty(Game *game, BoardSquare *square, DisasterType disaster);

int attemptRepairProperty(Player *player, BoardSquare *square);

void repairOwnedDamagedProperties(Game *game, Player *player);

const char *insuranceTypeName(InsuranceType type);

const char *disasterTypeName(DisasterType disaster);

void processDisasterIfNeeded(Game *game);



Money applyPercentageChange(Money value, int percentageChange);

int generateInflationRate(void);

void applyInflation(Game *game, int inflationRate);

void processInflationIfNeeded(Game *game);

void updateBankInterestForInflation(Game *game, int inflationRate);



void recalculatePropertyAfterDepreciation(BoardSquare *square);

void processPropertyDepreciation(Game *game);

int shouldRenovateProperty(Player *player, BoardSquare *square, Money renovationCost);

int renovateProperty(Player *player, BoardSquare *square);


void initializeEvents(Game *game);

void initializeNationalEventDeck(Game *game);

const char *nationalEventCardName(NationalEventCardType card);

void drawNationalEventCard(Game *game, Player *player);

void applyNationalEventCard(Game *game, Player *player, NationalEventCardType card);

int getNationalEventCardDuration(NationalEventCardType card);

int isNationalEventCardActive(Game *game, Player *player, NationalEventCardType card);

Money applyNationalConstructionCostModifiers(Game *game, Player *player, Money cost, int isHouse);

int applyNationalLoanInterestModifiers(Game *game, Player *player, int interestRate);

Money calculateNationalAdjustedAssetValue(Game *game, Player *player, BoardSquare *square);

void activateNationalEventCard(Game *game, Player *player, NationalEventCardType card);

void displayActiveNationalEventCards(Game *game);

void processNationalEventCardsEndOfRound(Game *game);



const char *economicEventName(EconomicEventType event);

void applyEconomicEvent(Game *game, EconomicEventType event);

void processEconomicEventIfNeeded(Game *game);



const char *governmentRegulationName(GovernmentRegulationType regulation);

void applyGovernmentRegulation(Game *game, GovernmentRegulationType regulation);

void processGovernmentRegulationIfNeeded(Game *game);

Money getCurrentHouseCost(Game *game, BoardSquare *square);

Money getCurrentHotelCost(Game *game, BoardSquare *square);

int getEffectiveBankLoanInterestRate(Game *game);

int countUndevelopedProperties(Game *game, Player *player);

void applyAntiSpeculationDeadline(Game *game, Player *player, BoardSquare *square);

void processAntiSpeculationDeadlines(Game *game);



void payTax(Game *game, Player *player, Money taxAmount, const char *taxName);

int recoverDebt(Game *game, Player *debtor, Player *creditor, Money amount, const char *reason);

void declareBankruptcy(Game *game, Player *player);

Money calculateTotalOwnedAssetValue(Game *game, Player *player);

Money calculateBuildingValue(Game *game, Player *player);

Money calculateMortgageLiability(Game *game, Player *player);

Money calculatePlayerNetWorth(Game *game, Player *player);

void checkAllPlayersSolvency(Game *game);

int countSolventPlayers(Game *game);

Player *selectWinner(Game *game);

void displayGameOver(Game *game);


int shouldPayJailBail(Player *player);

void resetAssetForBank(BoardSquare *square);

void applyAntiSpeculationToExistingProperties(Game *game);


const char *strategyName(StrategyType strategy);

void displayPlayerRoundSummary(Game *game, Player *player);

void displayRoundSummary(Game *game);

#endif  