package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"regexp"
	"strconv"
	"strings"
	"time"

	polygon "github.com/polygon-io/client-go/rest"
	"github.com/polygon-io/client-go/rest/models"
)

const (
	APIKEY = ""
)

func main() {
	if len(os.Args) != 5 {
		log.Fatalf("main.go :: Usage: go run main.go <MODE_NUMBER(0=Equities,1=Forex,2=Crypto)> <ASSET> <FROM_DATE(YYYY-MM-DD)> <TO_DATE(YYYY-MM-DD)>")
	}
	// First arg: mode number
	mode, err := strconv.Atoi(os.Args[1])
	if err != nil {
		log.Fatalf("main.go :: Parameter one, mode number must be an integer, got %q", os.Args[1])
	}
	if mode < 0 || mode > 4 {
		log.Fatalf("main.go :: Parameter one, Mode number must be an integer in range [0,4], got %d", mode)
	}
	// Second arg: asset symbol
	asset := strings.ToUpper(os.Args[2])
	onlyLetters := regexp.MustCompile(`^[A-Z]+$`)
	switch mode {
	case 0: // Equities
		if len(asset) > 4 {
			log.Fatalf("main.go :: Equities asset must be at most 4 letters (e.g. SPY, TSLA), got %q", asset)
		}
		if !onlyLetters.MatchString(asset) {
			log.Fatalf("main.go :: Equities asset must contain only letters, got %q", asset)
		}
	case 1: // Forex
		if len(asset) != 6 {
			log.Fatalf("main.go :: Forex asset must be 6 letters (e.g. EURUSD, USDMXN), got %q", asset)
		}
		if !onlyLetters.MatchString(asset) {
			log.Fatalf("main.go :: Forex asset must contain only letters, got %q", asset)
		}
	case 2: // Crypto
		if len(asset) < 6 || len(asset) > 7 {
			log.Fatalf("main.go :: Crypto asset must be 6 or 7 letters (e.g. BTCUSD, DOGEUSD), got %q", asset)
		}
		if !onlyLetters.MatchString(asset) {
			log.Fatalf("main.go :: Crypto asset must contain only letters, got %q", asset)
		}
	case 3: // Options
		if len(asset) > 18 {
			log.Fatalf("main.go :: Options asset must be  letters (e.g. SPY251219C00650000 -> SPY 2025 12/19 Expiration Call 650 Strike), got %q", asset)
		}
	case 4: // Futures
		if len(asset) != 4 {
			log.Fatalf("main.go :: Crypto asset must be 6 or 7 letters (e.g. GCJ2, VXJ6), got %q", asset)
		}
	}
	// Build symbol prefix depending on mode
	var symbol string
	switch mode {
	case 0: // Equities
		symbol = asset
	case 1: // Forex
		symbol = "C:" + asset
	case 2: // Crypto
		symbol = "X:" + asset
	case 3: // Options
		symbol = "O:" + asset
	case 4: // Futures
		symbol = asset
	}
	// Third arg: from date
	from, err := time.Parse("2006-01-02", os.Args[3])
	if err != nil {
		log.Fatalf("main.go :: Error parsing 'from' date (%s): %v", os.Args[3], err)
	}
	// Fourth arg: to date
	to, err := time.Parse("2006-01-02", os.Args[4])
	if err != nil {
		log.Fatalf("main.go :: Error parsing 'to' date (%s): %v", os.Args[4], err)
	}
	var closes []float64
	// Polygon client
	c := polygon.New(APIKEY)
	// Build query parameters
	if mode != 4 { // Equities, Cryptocurrencies, Forex and Options
		params := models.ListAggsParams{
			Ticker:     symbol,
			Multiplier: 1,
			Timespan:   "day",
			From:       models.Millis(from),
			To:         models.Millis(to),
		}.WithAdjusted(true).WithOrder(models.Order("asc")).WithLimit(120)
		// Run query
		iter := c.ListAggs(context.Background(), params)
		for iter.Next() {
			item := iter.Item()
			closes = append(closes, item.Close)
		}
		if iter.Err() != nil {
			log.Fatal("main.go :: " + iter.Err().Error())
		}
	} else { // Futures
		params := models.ListFuturesAggsParams{
			Ticker: symbol,
			Resolution: "1day",
		}.WithSort("window_start.desc").WithLimit(120)
		// Run query
		iter := c.ListFuturesAggs(context.Background(), params)
		for iter.Next() {
			item := iter.Item()
			closes = append(closes, item.Close)
		}
		if iter.Err() != nil {
			log.Fatal("main.go :: " + iter.Err().Error())
		}
	}
	fmt.Println("main.go :: Collected closes:", closes)
}
