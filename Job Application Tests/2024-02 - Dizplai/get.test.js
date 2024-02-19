const request = require("supertest");
const baseURL = "http://localhost:3000";

// Tests all GET functions
describe("GET /poll", () => {
    it("should return 200", async () => {
        const response = await request(baseURL).get("/poll");
        expect(response.statusCode).toBe(200);
    });
    it("should return all available polls", async () => {
        const response = await request(baseURL).get("/poll");
        expect(response.body.length >= 1).toBe(true);
    });
});

describe("GET /poll/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    it("should return 200 when parameter matches an existing pollId", async () => {
        const response = await request(baseURL).get("/poll/"+validPoll);
        expect(response.statusCode).toBe(200);
    });
    it("should return one poll with pollId matching given parameter", async () => {
        const response = await request(baseURL).get("/poll/"+validPoll);
        expect(response.body.length == 1).toBe(true);
    });
    it("should return 400 when parameter does not match an existing pollId", async () => {
        const response = await request(baseURL).get("/poll/"+invalidPoll);
        expect(response.statusCode).toBe(400);
    });
});

describe("GET /vote", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).get("/vote");
        expect(response.status).toBe(405);
    });
});

describe("GET /vote/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    const validVote = {
        "pollId": "plw2024",
        "optionId": 1
    };
    var voteId = "";
    beforeAll(async () => {
        // Ensure there is at least 1 vote before checking for votes
        const response = await request(baseURL).post("/vote").send(validVote);
        voteId = response.body;
    });
    afterAll(async () => {
        // Delete vote to prevent bloating
        const response = await request(baseURL).delete("/vote/"+voteId);
    });
    it("should return 200 when parameter matches an existing pollId", async () => {
        const response = await request(baseURL).get("/vote/"+validPoll);
        expect(response.status).toBe(200);
    });
    it("should return results of poll with pollId matching parameter", async () => {
        const response = await request(baseURL).get("/vote/"+validPoll);
        expect(response.body.length == 3).toBe(true);
    });
    it("should sort poll results in descending order of votes", async () => {
        const response = await request(baseURL).get("/vote/"+validPoll);
        expect(response.body[0][0] >= response.body[1][0] >= response.body[2][0]).toBe(true);
    });
    it("should return poll results as a percentage of the votes", async () => {
        const response = await request(baseURL).get("/vote/"+validPoll);
        expect(response.body[0][0] + response.body[1][0] + response.body[2][0]).toBe(100);
    });
    it("should return 400 when parameter does not match an existing pollId", async () => {
        const response = await request(baseURL).get("/vote/"+invalidPoll);
        expect(response.status).toBe(400);
    });
});

describe("GET /*", () => {
    const gibberish = "thisMakesNoSense";
    const validPoll = "plw2024";
    it("should return 200 for empty space after baseURL (gets main voting page)", async () => {
        const response = await request(baseURL).get("/");
        expect(response.status).toBe(200);
    });
    it("should return 404 if unexpected route given after baseURL", async () => {
        const response = await request(baseURL).get("/"+gibberish);
        expect(response.status).toBe(404);
    });
    it("should return 404 if valid pollId given after baseURL without '/poll/'", async () => {
        const response = await request(baseURL).get("/"+validPoll);
        expect(response.status).toBe(404);
    });
});