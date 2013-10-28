#include <gtest/gtest.h>

#include <string>
#include <set>
#include <vector>

#include "mock_facility.h"
#include "request.h"
#include "request_portfolio.h"
#include "bid.h"
#include "bid_portfolio.h"
#include "resource.h"
#include "test_context.h"

#include "exchange_context.h"

using std::string;
using std::set;
using std::vector;

using cyclus::ExchangeContext;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::Resource;
using cyclus::TestContext;
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ExchangeContextTests: public ::testing::Test {
 protected:
  TestContext tc;
  MockFacility* fac1;
  MockFacility* fac2;
  Request<Resource>::Ptr req1, req2;
  RequestPortfolio<Resource> rp1, rp2;
  string commod1, commod2;
  
  virtual void SetUp() {
    fac1 = new MockFacility(tc.get());
    fac2 = new MockFacility(tc.get());

    commod1 = "commod1";
    req1 = Request<Resource>::Ptr(new Request<Resource>());
    req1->commodity = commod1;
    req1->requester = fac1;
    
    req2 = Request<Resource>::Ptr(new Request<Resource>());
    req2->commodity = commod1;
    req2->requester = fac2;

    rp1.AddRequest(req1);    
    rp2.AddRequest(req2);
  };
  
  virtual void TearDown() {
    rp1.Clear();
    rp2.Clear();
    delete fac1;
    delete fac2;
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, Empty) {
  ExchangeContext<Resource> context;
  EXPECT_TRUE(context.requests().empty());
  EXPECT_TRUE(context.RequestsForCommod(commod2).empty());
  EXPECT_TRUE(context.RequestsForCommod(commod2).empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest1) {
  // 1 request for 1 commod
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);

  std::vector<RequestPortfolio<Resource> > vp;
  vp.push_back(rp1);
  EXPECT_EQ(vp, context.requests());
  
  EXPECT_EQ(1, context.RequestsForCommod(commod1).size());  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest2) {
  // 2 requests for 1 commod
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);
  context.AddRequestPortfolio(rp2);

  std::vector<RequestPortfolio<Resource> > vp;
  vp.push_back(rp1);
  vp.push_back(rp2);
  EXPECT_EQ(vp, context.requests());
  
  EXPECT_EQ(2, context.RequestsForCommod(commod1).size());  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  vr.push_back(req2);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddRequest3) {
  // 2 requests for 2 commod
  Request<Resource>::Ptr req = Request<Resource>::Ptr(new Request<Resource>());
  req->commodity = commod2;
  req->requester = fac1;
  rp1.AddRequest(req);
  
  ExchangeContext<Resource> context;
    
  context.AddRequestPortfolio(rp1);
  
  EXPECT_EQ(1, context.RequestsForCommod(commod1).size());
  EXPECT_EQ(1, context.RequestsForCommod(commod2).size());
  
  std::vector<Request<Resource>::Ptr> vr;
  vr.push_back(req1);
  EXPECT_EQ(vr, context.RequestsForCommod(commod1));

  vr.clear();
  vr.push_back(req);
  EXPECT_EQ(vr, context.RequestsForCommod(commod2));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddBid1) {
  // bid bid for a request
  ExchangeContext<Resource> context;
  context.AddRequestPortfolio(rp1);

  EXPECT_TRUE(context.BidsForRequest(req1).empty());
  
  Bid<Resource>::Ptr bid = Bid<Resource>::Ptr(new Bid<Resource>());
  bid->request = req1;
  BidPortfolio<Resource> bp1;
  bp1.AddBid(bid);

  context.AddBidPortfolio(bp1);

  std::vector<BidPortfolio<Resource> > vp;
  vp.push_back(bp1);
  EXPECT_EQ(vp, context.bids());

  EXPECT_EQ(1, context.BidsForRequest(req1).size());
  
  std::vector<Bid<Resource>::Ptr> vr;
  vr.push_back(bid);
  EXPECT_EQ(vr, context.BidsForRequest(req1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ExchangeContextTests, AddBid2) {
  // multiple bids for multiple requests
  ExchangeContext<Resource> context;
  context.AddRequestPortfolio(rp1);
  context.AddRequestPortfolio(rp2);

  EXPECT_TRUE(context.BidsForRequest(req1).empty());
  EXPECT_TRUE(context.BidsForRequest(req2).empty());

  // bid1 and bid2 are from one bidder (fac1)
  BidPortfolio<Resource> bp1;
  Bid<Resource>::Ptr bid1 = Bid<Resource>::Ptr(new Bid<Resource>());
  bid1->request = req1;
  bid1->bidder = fac1;
  bp1.AddBid(bid1);
  Bid<Resource>::Ptr bid2 = Bid<Resource>::Ptr(new Bid<Resource>());
  bid2->request = req2;
  bid2->bidder = fac1;
  bp1.AddBid(bid2);
  
  // bid3 and bid4 are from one bidder (fac2)
  BidPortfolio<Resource> bp2;
  Bid<Resource>::Ptr bid3 = Bid<Resource>::Ptr(new Bid<Resource>());
  bid3->request = req1;
  bid3->bidder = fac2;
  bp2.AddBid(bid3);
  Bid<Resource>::Ptr bid4 = Bid<Resource>::Ptr(new Bid<Resource>());
  bid4->request = req2;
  bid4->bidder = fac2;
  bp2.AddBid(bid4);

  std::vector<BidPortfolio<Resource> > vp;
  std::vector<Bid<Resource>::Ptr> vreq1;
  std::vector<Bid<Resource>::Ptr> vreq2;

  // add bids from first bidder
  context.AddBidPortfolio(bp1);

  vp.push_back(bp1);
  EXPECT_EQ(vp, context.bids());
  
  vreq1.push_back(bid1);
  vreq2.push_back(bid2);
  EXPECT_EQ(1, context.BidsForRequest(req1).size());
  EXPECT_EQ(1, context.BidsForRequest(req2).size());
  EXPECT_EQ(vreq1, context.BidsForRequest(req1));
  EXPECT_EQ(vreq2, context.BidsForRequest(req2));
  
  // add bids from second bidder
  context.AddBidPortfolio(bp2);

  vp.push_back(bp2);
  EXPECT_EQ(vp, context.bids());
  
  vreq1.push_back(bid3);
  vreq2.push_back(bid4);
  EXPECT_EQ(2, context.BidsForRequest(req1).size());
  EXPECT_EQ(2, context.BidsForRequest(req2).size());
  EXPECT_EQ(vreq1, context.BidsForRequest(req1));
  EXPECT_EQ(vreq2, context.BidsForRequest(req2));
}
