var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

/* GET home page. */
router.get('/online', function(req, res, next) {
  res.render('online', { title: 'Express' });
});


/* GET offline page. */
router.get('/offline', function(req, res, next) {
  res.render('offline', { title: 'Express' });
});

module.exports = router;
