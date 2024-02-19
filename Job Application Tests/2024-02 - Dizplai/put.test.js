const request = require("supertest");
const baseURL = "http://localhost:3000";

// Tests all GET functions
describe("PUT /poll", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).put("/poll");
        expect(response.statusCode).toBe(405);
    });
});

describe("PUT /poll/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    it("should return 405 on existing pollId", async () => {
        const response = await request(baseURL).put("/poll/"+validPoll);
        expect(response.statusCode).toBe(405);
    });
    it("should return 405 on invalid pollId", async () => {
        const response = await request(baseURL).put("/poll/"+invalidPoll);
        expect(response.statusCode).toBe(405);
    });
});

describe("PUT /vote", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).put("/vote");
        expect(response.statusCode).toBe(405);
    });
});

describe("PUT /vote/:id", () => {
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
    it("should return 405 even when parameter matches an existing pollId", async () => {
        const response = await request(baseURL).put("/vote/"+validPoll);
        expect(response.status).toBe(405);
    });
    it("should return 405 when parameter does not match an existing pollId", async () => {
        const response = await request(baseURL).put("/vote/"+invalidPoll);
        expect(response.status).toBe(405);
    });
});


describe("PUT /*", () => {
    const gibberish = "thisMakesNoSense";
    const validPoll = "plw2024";
    it("should return 404 for empty space after baseURL", async () => {
        const response = await request(baseURL).post("/");
        expect(response.status).toBe(404);
    });
    it("should return 404 if unexpected route given after baseURL", async () => {
        const response = await request(baseURL).post("/"+gibberish);
        expect(response.status).toBe(404);
    });
    it("should return 404 if valid pollId given after baseURL without '/poll/'", async () => {
        const response = await request(baseURL).post("/"+validPoll);
        expect(response.status).toBe(404);
    });
});