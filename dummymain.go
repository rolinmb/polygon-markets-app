package main

import (
	"context"
	"log"
	"time"

	polygon "github.com/polygon-io/client-go/rest"
	"github.com/polygon-io/client-go/rest/models"
)

func main() {
	c := polygon.New("YOURAPIKEY")
	from, err := time.Parse("2006-01-02", "2025-01-01")
    if err != nil {
        log.Fatalf("Error parsing 'from' date: %v", err)
    }
    to, err := time.Parse("2006-01-02", "2025-09-19")
    if err != nil {
        log.Fatalf("Error parsing 'to' date: %v", err)
    }
	params := models.ListAggsParams{
		Ticker:     "C:EURUSD",
		Multiplier: 1,
		Timespan:   "day",
		From:       models.Millis(from),
		To:         models.Millis(to),
	}.
		WithAdjusted(true).
		WithOrder(models.Order("asc")).
		WithLimit(120)
	iter := c.ListAggs(context.Background(), params)
	for iter.Next() {
		log.Print("main.go :: " + iter.Item())
	}
	if iter.Err() != nil {
		log.Fatal(iter.Err())
	}
}
