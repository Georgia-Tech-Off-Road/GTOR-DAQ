const express = require("express")
var router = express.Router();

router.get("/", function(req, res) {
    res.send("Andrew is the GOAT!");
});

module.exports = router;