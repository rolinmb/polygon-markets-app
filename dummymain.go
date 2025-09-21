package main

import (
	"os"
	"fmt"
	"log"
	"time"
	"strings"
	"context"

	polygon "github.com/polygon-io/client-go/rest"
	"github.com/polygon-io/client-go/rest/models"
)

const (
	APIKEY = "yOURAPIKEY"
)

func main() {
	if len(os.Args) != 4 {
		log.Fatalf("Usage: go run %s <CURRENCY_PAIR(6 letters)> <FROM_DATE(YYYY-MM-DD)> <TO_DATE(YYYY-MM-DD)>", os.Args[0])
	}
	// First argument: forex pair (must be 6 letters, e.g. EURUSD)
	pair := os.Args[1]
	if len(pair) != 6 {
		log.Fatalf("Currency pair must be 6 characters, e.g. EURUSD, got %q", pair)
	}
	pair = strings.ToUpper(pair)
	ticker := "C:" + pair
	// Second argument: from date
	from, err := time.Parse("2006-01-02", os.Args[2])
	if err != nil {
		log.Fatalf("Error parsing 'from' date (%s): %v", os.Args[2], err)
	}
	// Third argument: to date
	to, err := time.Parse("2006-01-02", os.Args[3])
	if err != nil {
		log.Fatalf("Error parsing 'to' date (%s): %v", os.Args[3], err)
	}
	// Polygon client
	c := polygon.New(APIKEY)
	// Build query parameters
	params := models.ListAggsParams{
		Ticker:     ticker,
		Multiplier: 1,
		Timespan:   "day",
		From:       models.Millis(from),
		To:         models.Millis(to),
	}.WithAdjusted(true).WithOrder(models.Order("asc")).WithLimit(120)

	// Run query
	iter := c.ListAggs(context.Background(), params)

	for iter.Next() {
		item := iter.Item()
		fmt.Printf("%s :: %+v\n", ticker, item)
	}

	if iter.Err() != nil {
		log.Fatal(iter.Err())
	}
}
