package main

import (
    "context"
    "encoding/json"
    "log"
    "net/http"
    "os"
    "os/signal"
    "time"

    "github.com/go-redis/redis/v8"
)

var rdb *redis.Client

func profileHandler(w http.ResponseWriter, r *http.Request) {
    id := r.URL.Query().Get("id")
    ctx := context.Background()
    val, err := rdb.Get(ctx, "user:"+id).Result()
    if err == redis.Nil {
        // fallback to DB (omitted)
        http.Error(w, "not found", 404)
        return
    } else if err != nil {
        http.Error(w, "server error", 500)
        return
    }
    var obj map[string]interface{}
    json.Unmarshal([]byte(val), &obj)
    json.NewEncoder(w).Encode(obj)
}

func main() {
    rdb = redis.NewClient(&redis.Options{Addr: "redis:6379"})

    srv := &http.Server{Addr: ":8080"}
    http.HandleFunc("/profile", profileHandler)

    go func() {
        if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed { log.Fatal(err) }
    }()

    stop := make(chan os.Signal, 1)
    signal.Notify(stop, os.Interrupt)
    <-stop
    ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
    defer cancel()
    srv.Shutdown(ctx)
}
